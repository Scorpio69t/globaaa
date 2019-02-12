/*
 *   FILE: uthread.c
 * AUTHOR: peter demoreuille
 *  DESCR: userland threads
 *   DATE: Sun Sep 30 23:45:00 EDT 2001
 *
 * Modifed considerably by Tom Doeppner in support of two-level threading
 *   DATE: Sun Jan 10, 2016
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include "uthread.h"
#include "uthread_private.h"
#include "uthread_queue.h"
#include "uthread_bool.h"
#include "uthread_sched.h"
#include "uthread_mtx.h"
#include "uthread_cond.h"


/* ---------- globals -- */

__thread uthread_t*    ut_curthr = 0;       /* per-LWP current running thread */
__thread lwp_t*        curlwp;              /* per-LWP reference to the LWP context info */
uthread_t   uthreads[UTH_MAX_UTHREADS];     /* threads on the system */
extern sigset_t    VTALRMmask;              /* clock interrupt mask */

static utqueue_t        reap_queue;         /* dead threads */
static uthread_id_t     reaper_thr_id;      /* reference to reaper thread */
static uthread_mtx_t    reap_mtx;
extern pthread_mutex_t  runq_mtx;

pthread_mutexattr_t merrorattr;


/* ---------- prototypes -- */

static void create_first_thr(uthread_func_t firstFunc, long argc, char *argv[]);

static uthread_id_t uthread_alloc(void);
static void uthread_destroy(uthread_t *thread);

static char *alloc_stack(void);
static void free_stack(char *stack);

static void reaper_init(void);
static void reaper(long a0, char *a1[]);
static void make_reapable(uthread_t *uth);
static void uthread_start_lwp(void);


/* ---------- public code -- */

/*
 * uthread_start
 *
 * This initializes everything, then becomes the first uthread and invokes its first
 * function. It does not return -- when all uthreads are done, the reaper calls exit.
 */
void
uthread_start(uthread_func_t firstFunc, long argc, char *argv[])
{
    int i;
    for (i=0; i<UTH_MAX_UTHREADS; i++) {
        uthreads[i].ut_state = UT_NO_STATE;
        uthreads[i].ut_id = i;
    }

    pthread_mutexattr_init(&merrorattr);
    pthread_mutexattr_settype(&merrorattr, PTHREAD_MUTEX_ERRORCHECK);

    if ((i = pthread_mutex_init(&runq_mtx, &merrorattr)) != 0) {
        fprintf(stderr, strerror(i));
        exit(1);
    }

    /* these should go last, and in this order */
    uthread_sched_init(); // masks clock interrupts
    reaper_init();
    create_first_thr(firstFunc, argc, argv);
}



/*
 * uthread_create
 *
 * Create a uthread to execute the specified function <func> with arguments
 * <arg1> and <arg2> and initial priority <prio>. To do this, you should first
 * find a valid (unused) id for the thread using uthread_alloc (failing this,
 * return an error).  Next, create a context for the thread to execute on using
 * uthread_makecontext(), set up the uthread_t struct corresponding to the
 * newly-found id, make the thread runnable by calling uthread_startonrunq, and
 * return the aforementioned thread id in <uidp>.  Return 0 on success, -1 on error.
 */
int
uthread_create(uthread_id_t *uidp, uthread_func_t func,
               long arg1, void *arg2, int prio)
{
    uthread_id_t tid = uthread_alloc();
    *uidp = tid;
    if (tid == -1) {
        return -1;
    }

    uthread_t *thr = &uthreads[tid];
    thr->ut_stack = alloc_stack();
    if (thr->ut_stack == NULL) {
        *uidp = -1;
        return -1;
    }

    uthread_makecontext(&thr->ut_ctx, thr->ut_stack, UTH_STACK_SIZE, func, arg1, arg2);
    memset(&thr->ut_link, 0, sizeof(list_link_t));
    thr->ut_prio = -1;  // illegal value, forcing it to be changed in uthread_setprio
    thr->ut_errno = thr->ut_has_exited = thr->ut_exit = thr->ut_detached = 0;
    utqueue_init(&thr->ut_waiter);
    pthread_mutex_init(&thr->ut_pmut, &merrorattr);

    // Thread must start with preemption disabled: this is assumed in uthread_switch
    thr->ut_no_preempt_count = 1;

    uthread_startonrunq(tid, prio); // makes new thread runnable

    return 0;
}



/*
 * uthread_exit
 *
 * Terminate the current thread.  Should set all the related flags and
 * such in uthread_t.
 *
 * If this is not a detached thread, and there is someone
 * waiting to join with it, you should wake up that thread.
 *
 * If the thread is detached, it should be put onto the reaper's dead
 * thread queue and wakeup the reaper thread by calling make_reapable().
 */
void
uthread_exit(int status)
{
    // TODO:
    // 1. Lock reap_mtx to synchronize access to the reap queue. Note
    // that the calling thread will eventually be put on this queue.
    //
    // 2. Make sure that the current uthread's resources aren't
    // destroyed before the current thread switches away from its
    // context.

    uthread_nopreempt_on();

    ut_curthr->ut_state = UT_ZOMBIE;
    ut_curthr->ut_has_exited = 1;
    ut_curthr->ut_exit = status;

    if (ut_curthr->ut_detached) {
        uthread_mtx_lock(&reap_mtx);
	/* (1): lock after reap_mtx - otherwise you'll try to unlock in another process,
	 * which will throw a pthread error. */
        pthread_mutex_lock(&ut_curthr->ut_pmut);
        make_reapable(ut_curthr);	/* requires reap_mtx and ut_pmut locked */
    } else {
        pthread_mutex_lock(&ut_curthr->ut_pmut);
        uthread_t *waiter = utqueue_dequeue(&ut_curthr->ut_waiter);
        if (waiter != NULL) {
            uthread_wake(waiter);
        }

        /* TODO: call uthread_switch here; I think queue value should be NULL
         * since I don't want to run this thread again. Make sure to unlock
	 * the thread's ut_pmut mutex so that the thread can be reaped after it
	 * switches off its stack.
	 */
        uthread_switch(NULL, 0, &ut_curthr->ut_pmut);
    }

    PANIC("returned to a dead thread");
}



/*
 * uthread_join
 *
 * Wait for the given thread to finish executing. If the thread has not
 * finished executing, the calling thread needs to block until this event
 * happens.
 *
 * Error conditions include (but are not limited to):
 * o the thread described by <uid> does not exist
 * o two threads attempting to join the same thread, etc..
 * Return an appropriate error code (found in manpage for pthread_join) in
 * these situations (and more).
 *
 * Note that if a thread finishes executing and is never uthread_join()'ed
 * (or uthread_detach()'ed) it remains in the state UT_ZOMBIE and is never
 * cleaned up.
 *
 * When you have successfully joined with the thread, set its ut_detached
 * flag to true, and then wake the reaper so it can cleanup the thread by
 * calling make_reapable
 */
int
uthread_join(uthread_id_t uid, int *return_value)
{
    // TODO:
    // 1. Make sure the thread with thread id uid is modified in a
    // pthread-safe manner. Don't forget about clock interrupts!
    //
    // 2. Make sure that both the uthread mutex isn't released until
    // and the current thread is put on the target thread's waiters
    // queue until after this thread context-switch. Otherwise it is
    // possible for the target thread to exit and wake up the calling
    // thread on another processor, resulting in a situation in which
    // both threads are executing on separate processors with the same
    // stack.

    if (uid < 0 || uid >= UTH_MAX_UTHREADS)
        return -1;

    uthread_t *thr = &uthreads[uid];
    uthread_nopreempt_on();

    /* In almost all functions you initially lock the thread's ut_pmut.
     * This is important not only to ensure that the thread isn't reaped
     * until it's switched off its stack, but in this case it'll make sure
     * that no other uthread functions modify thr's struct fields
     * while in this function */

    pthread_mutex_lock(&thr->ut_pmut);

    if (!utqueue_empty(&thr->ut_waiter)) {
        // some other thread is joining
        pthread_mutex_unlock(&thr->ut_pmut);
        uthread_nopreempt_off();
        return -1;
    }
    if (thr->ut_detached) {
        pthread_mutex_unlock(&thr->ut_pmut);
        uthread_nopreempt_off();
        return -1;
    }
    if (thr->ut_state != UT_ZOMBIE) {
        // target hasn't terminated yet. Atomically put caller on target's waiter q and
        // yield LWP
        ut_curthr->ut_state = UT_WAIT;

        // TODO: Call uthread_switch here.
	uthread_switch(&thr->ut_waiter, 0, &thr->ut_pmut);
    } else {
        // target has already terminated.
        pthread_mutex_unlock(&thr->ut_pmut);
    }
    // When we get here, we know the target has terminated.
    // There's a problem if another thread concurrently attempts to join, but the effect of this is
    // officially undefined
    assert(thr->ut_state == UT_ZOMBIE);
    uthread_nopreempt_off();
    if (return_value) {
        *return_value = thr->ut_exit;
    }

    uthread_mtx_lock(&reap_mtx); // must be locked before calling make_reapable. See uthread_exit.
    pthread_mutex_lock(&thr->ut_pmut);
    make_reapable(thr);
    return 0;
}



/*
 * uthread_detach
 *
 * Detach the given thread. Thus, when this thread's function has finished
 * executing, no other thread need (or should) call uthread_join() to perform
 * the necessary cleanup.
 *
 * There is also the special case if the thread has already exited and then
 * is detached (i.e. was already in the state UT_ZOMBIE when uthread_deatch()
 * is called). In this case it is necessary to call make_reapable on the
 * appropriate thread.
 *
 * There are also some errors to check for, see the man page for
 * pthread_detach (basically just invalid threads, etc).
 *
 */
int
uthread_detach(uthread_id_t uid)
{
    // TODO: Something has to be done to ensure that the reaper
    // doesn't destroy the stack we're executing on too early.
    //
    // Hint: See the reaper code, and note that reap_queue
    // access needs to be synchronized

    if (uid < 0 || uid >= UTH_MAX_UTHREADS)
        return -1;
    uthread_t *thr = &uthreads[uid];
    uthread_nopreempt_on();
    thr->ut_detached = 1;
    if (thr->ut_state == UT_ZOMBIE) {
	uthread_mtx_lock(&reap_mtx);
	pthread_mutex_lock(&thr->ut_pmut);
        make_reapable(thr);
    }
    uthread_nopreempt_off();
    return 0;
}



/*
 * uthread_self
 *
 * Returns the id of the currently running thread.
 */
uthread_id_t
uthread_self(void)
{
    uthread_nopreempt_on();
    assert(ut_curthr != NULL);
    uthread_id_t id = ut_curthr->ut_id;
    uthread_nopreempt_off();
    return id;
}




/* ------------- private code -- */



/*
 * uthread_alloc
 *
 * find a free uthread_t, returns the id.
 */
static uthread_id_t
uthread_alloc(void)
{
    static uthread_id_t PrevID = -1;
    int i;
    int start = PrevID+1;
    if (start >= UTH_MAX_UTHREADS)
        start = 0;
    int end = UTH_MAX_UTHREADS;
    uthread_nopreempt_on();
    while (1) {
        for (i=start; i<end; i++) {
            if (uthreads[i].ut_state == UT_NO_STATE) {
                uthreads[i].ut_state = UT_TRANSITION;
                PrevID = i;
                uthread_nopreempt_off();
                return(PrevID);
            }
        }
        if (start > 0) {
            end = start;
            start = 0;
            continue;
        }
        break;
    }
    uthread_nopreempt_off();
    return -1;
}

/*
 * uthread_destroy
 *
 * Cleans up resources associated with a thread (since it's now finished
 * executing). This is called implicitly whenever a detached thread finishes
 * executing or whenever non-detached thread is uthread_join()'d.
 */
static void
uthread_destroy(uthread_t *uth)
{
    assert(uth->ut_state == UT_ZOMBIE);
    free_stack(uth->ut_stack);
    uth->ut_errno = uth->ut_has_exited = uth->ut_exit = uth->ut_detached = 0;
    utqueue_init(&uth->ut_waiter);
    pthread_mutex_destroy(&uth->ut_pmut);
    uth->ut_state = UT_NO_STATE;
}


static uthread_cond_t reap_cond;

/*
 * reaper_init
 * initialize the reap queue
 */
static void
reaper_init(void)
{
    utqueue_init(&reap_queue);
    uthread_mtx_init(&reap_mtx);
    uthread_cond_init(&reap_cond);
}


/*
 * reaper
 *
 * this is responsible for going through all the threads on the dead
 * threads list (which should all be in the ZOMBIE state) and then
 * cleaning up all the threads that have been detached/joined with
 * already.
 *
 * in addition, when there are no more runnable threads (besides the
 * reaper itself) it will call exit() to stop the program.
 */
static void
reaper(long a0, char *a1[])
{
    uthread_nopreempt_on();
    uthread_mtx_lock(&reap_mtx); // get exclusive access to reap queue

    while(1) {
        uthread_t *thread;
        int th;

        while(utqueue_empty(&reap_queue)) {
            // wait for a thread to join the reap queue
            uthread_cond_wait(&reap_cond, &reap_mtx);
        }

        while (!utqueue_empty(&reap_queue)) {
            // deal with all threads on reap queue
            thread = utqueue_dequeue(&reap_queue);


            assert(thread->ut_state == UT_ZOMBIE);
            pthread_mutex_lock(&thread->ut_pmut); // wait for thread to get off its stack: pmut is
                                                  // unlocked when the thread has switched to the LWP's stack.
            pthread_mutex_unlock(&thread->ut_pmut);  // safe to unlock it -- the thread is effectively gone.
            uthread_destroy(thread);
        }

        /* check and see if there are still runnable threads */
        for (th = 0; th < UTH_MAX_UTHREADS; th++)
        {
            if (th != reaper_thr_id &&
                uthreads[th].ut_state != UT_NO_STATE)
            {
                break;
            }
        }

        if (th == UTH_MAX_UTHREADS)
        {
            /* we leak the reaper's stack */
            fprintf(stderr, "mthreads: no more threads.\n");
            fprintf(stderr, "mthreads: bye!\n");
            exit(0);
        }
    }
}

void lwp_switch(void);
extern pthread_mutex_t runq_mtx;
void uthread_runq_enqueue(uthread_t *thr);


/*
 * Turns the main context (the 'main' routine that initialized
 * this process) into a regular uthread that can be switched
 * into and out of. Must be called from the main context (i.e.,
 * by uthread_start()). Also creates reaper thread and sets up
 * contexts for all LWPs
 */
static void
create_first_thr(uthread_func_t firstFunc, long argc, char *argv[])
{
    // Set up uthread context for main thread

    // This does what uthread_alloc does, but without a valid ut_curthr
    uthread_id_t tid = 0;
    uthreads[0].ut_state = UT_TRANSITION;

    ut_curthr = &uthreads[tid];
    memset(&ut_curthr->ut_link, 0, sizeof(list_link_t));
    ut_curthr->ut_prio = UTH_MAXPRIO;
    ut_curthr->ut_errno = ut_curthr->ut_has_exited = ut_curthr->ut_exit = 0;
    ut_curthr->ut_detached = 1;
    utqueue_init(&ut_curthr->ut_waiter);
    ut_curthr->ut_state = UT_RUNNABLE;
    pthread_mutex_init(&ut_curthr->ut_pmut, &merrorattr);
    ut_curthr->ut_no_preempt_count = 1; // thread created with clock interrupts masked
    ut_curthr->ut_stack = alloc_stack();
    if (ut_curthr->ut_stack == NULL) {
        PANIC("Could not create stack for first thread.");
    }

    uthread_makecontext(&ut_curthr->ut_ctx, ut_curthr->ut_stack, UTH_STACK_SIZE,
                        firstFunc, argc, argv);

    // Thread must start with preemption disabled: this is assumed in uthread_switch
    ut_curthr->ut_no_preempt_count = 1;

    uthread_runq_enqueue(ut_curthr);

    // first thread is now on run q; next step is to set up the reaper thread


    reaper_thr_id = uthread_alloc();
    ut_curthr = &uthreads[reaper_thr_id];
    memset(&ut_curthr->ut_link, 0, sizeof(list_link_t));
    ut_curthr->ut_prio = UTH_MAXPRIO;
    ut_curthr->ut_errno = ut_curthr->ut_has_exited = ut_curthr->ut_exit = 0;
    ut_curthr->ut_detached = 1;
    utqueue_init(&ut_curthr->ut_waiter);
    ut_curthr->ut_state = UT_RUNNABLE;
    pthread_mutex_init(&ut_curthr->ut_pmut, &merrorattr);
    ut_curthr->ut_no_preempt_count = 1; // thread created with clock interrupts masked
    ut_curthr->ut_stack = alloc_stack();
    if (ut_curthr->ut_stack == NULL) {
        PANIC("Could not create stack for reaper thread.");
    }

    uthread_makecontext(&ut_curthr->ut_ctx, ut_curthr->ut_stack, UTH_STACK_SIZE,
                        reaper, 0, 0);

    // Thread must start with preemption disabled: this is assumed in uthread_switch
    ut_curthr->ut_no_preempt_count = 1;

    uthread_runq_enqueue(ut_curthr);

    // reaper thread is now on run q; next step is to set up the initial LWP

    pthread_mutex_lock(&runq_mtx);  // lock runq so that no LWPs do anything till we're ready

    // set up additional LWPs: all inherit clock interrupt mask
    uthread_start_lwp();
    uthread_start_lwp();
    uthread_start_lwp();
    uthread_start_lwp();
    uthread_start_lwp();
    uthread_start_lwp();

    curlwp = (lwp_t *)malloc(sizeof(lwp_t));
    memset(curlwp, 0, sizeof(lwp_t));
    curlwp->ptid = pthread_self();
    lwp_cnt = 1;
    ut_curthr = NULL; // at the moment we don't have a current uthread -- they're both
                      // on the run q.
    lwp_switch();     // will unlock runq and start things off
    PANIC("lwp returned to create_first_thr");
}

/*
 * Adds the given thread to the reaper's queue, and wakes up the reaper.
 * Called when a thread is completely dead (is detached and exited).
 *
 */

static void
make_reapable(uthread_t *uth)
{
    // TODO:
    // 1. This function should be called with the reap_mtx locked
    // to synchronize access to the reap_queue with the reaper uthread.
    // You should figure out where to unlock this reap_mtx in this function.
    //
    // 2. You need to make sure that thread resources aren't destroyed
    // too early. See uthread_destroy() in the reaper code, what other
    // mutex needs to be locked by the caller of this function (in addition
    // to reap_mtx)?
    //
    // Hint: Consider ALL possible uthread inputs to this function!

    assert(uth->ut_state == UT_ZOMBIE);

    uthread_nopreempt_on();

    /* calling make_reapable on different thread */
    if(uth != ut_curthr) {
      utqueue_enqueue(&reap_queue, uth);
      uthread_cond_signal(&reap_cond);
      uthread_mtx_unlock(&reap_mtx);
      pthread_mutex_unlock(&uth->ut_pmut);
    }
    else {
      utqueue_enqueue(&reap_queue, uth);
      uthread_cond_signal(&reap_cond);
      uthread_mtx_unlock(&reap_mtx);
      uthread_switch(NULL, 0, &uth->ut_pmut);	/* switch out of current, detached thread */

      PANIC("returned to a dead thread");
    }

    uthread_nopreempt_off();
}

static char *
alloc_stack(void)
{
    uthread_nopreempt_on();
    char *stack = (char *)malloc(UTH_STACK_SIZE);
    uthread_nopreempt_off();
    return stack;
}

static void
free_stack(char *stack)
{
    uthread_nopreempt_on();
    free(stack);
    uthread_nopreempt_off();
}

/*
 * lwp_start
 *
 * The initial routine of all but the first LWP. It sets up its lwp struct. Both ut_curth and
 * curlwp are thread-local storage and thus private to the LWP. The former refers to the current
 * uthread (if any) being run by the LWP, the latter refers the LWPs private context info.
 */

static void *
lwp_start(void *dummy) {
    ut_curthr = NULL;
    curlwp = (lwp_t *)malloc(sizeof(lwp_t));
    memset(curlwp, 0, sizeof(lwp_t));
    curlwp->ptid = pthread_self();
    pthread_mutex_lock(&runq_mtx);
    lwp_cnt++;
    lwp_switch();
    PANIC("lwp returned to lwp_start");
    return 0;
}

/*
 * uthread_start_lwp
 *
 * Create a new LWP as a low-level thread to run uthreads.
 * Any number of these may be created.
 */

static void
uthread_start_lwp() {
    pthread_t ptid;
    pthread_create(&ptid, 0, lwp_start, 0);
    pthread_detach(ptid);
}
