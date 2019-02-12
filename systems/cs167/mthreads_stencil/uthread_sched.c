/*
 *   FILE: uthread_sched.c
 * AUTHOR: Peter Demoreuille
 *  DESCR: scheduling wack for uthreads
 *   DATE: Mon Oct  1 00:19:51 2001
 *
 * Modifed considerably by Tom Doeppner in support of two-level threading
 *   DATE: Sun Jan 10, 2016
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

#include "uthread.h"
#include "uthread_private.h"
#include "uthread_ctx.h"
#include "uthread_queue.h"
#include "uthread_bool.h"
#include "uthread_sched.h"

void uthread_runq_enqueue(uthread_t *thr);
static void uthread_runq_requeue(uthread_t *thr, int oldpri);
static int is_masked();

/* ---------- globals -- */


pthread_mutex_t runq_mtx;
static utqueue_t runq_table[UTH_MAXPRIO + 1]; /* priority runqueues */

/* ----------- public code -- */


/*
 * uthread_yield
 *
 * Causes the currently running thread to yield use of the processor to
 * another thread. The thread is still runnable however, so it should
 * be in the UT_RUNNABLE state and schedulable by the scheduler. When this
 * function returns, the thread should be executing again. A bit more clearly,
 * when this function is called, the current thread stops executing for some
 * period of time (allowing another thread to execute). Then, when the time
 * is right (ie when a call to uthread_switch() results in this thread
 * being swapped in), the function returns.
 */
void
uthread_yield()
{

    // TODO: Make sure that the current thread is not put on the run queue
    // until after the thread has context-switched. If we were to enqueue
    // the thread, and then context switch, there would be a window during
    // which this thread may be scheduled on another processor, resulting in
    // a situation in which two processors are executing on the same stack.
    //
    // Hint: See uthread_switch arguments

    uthread_nopreempt_on();
    assert(ut_curthr->ut_link.l_next == NULL);

    // make sure thread is runnable; mark it to be saved on runq
    ut_curthr->ut_state = UT_RUNNABLE;

    // lock the thread to prevent another processor from accessing
    pthread_mutex_lock(&ut_curthr->ut_pmut);
    uthread_switch(NULL, 1, &ut_curthr->ut_pmut);

    uthread_nopreempt_off();
}


/*
 * uthread_block
 *
 * Put the current thread to sleep, pending an appropriate call to
 * uthread_wake().
 */
void
uthread_block()
{
    uthread_nopreempt_on();
    ut_curthr->ut_state = UT_WAIT;
    //TODO: Call uthread_switch here
    uthread_switch(NULL, 0, NULL);	/* assume has been put on queue elsewhere */
    uthread_nopreempt_off();
}


/*
 * uthread_wake
 *
 * Wakes up the supplied thread (schedules it to be run again).  The
 * thread may already be runnable or already on cpu, so make sure to only mess with it if
 * it is actually in a wait state.
 */
void
uthread_wake(uthread_t *uthr)
{

    // TODO: Synchronize access to the run queue. This will ensure that
    // the target thread is still waiting after a call to uthread_block.
    //
    // Hint: Use runq_mtx

    assert(uthr->ut_state != UT_NO_STATE);
    uthread_nopreempt_on();
    pthread_mutex_lock(&runq_mtx);
    if (uthr->ut_state == UT_WAIT) {
       uthr->ut_state = UT_RUNNABLE;
        uthread_runq_enqueue(uthr);
        if ((uthr->ut_prio > ut_curthr->ut_prio) && !is_masked()) {
            // yield if its priority is better than caller's
            uthread_yield();
        }
        uthread_nopreempt_off();
    } else {
        uthread_nopreempt_off();
    }
    pthread_mutex_unlock(&runq_mtx);
}


/*
 * uthread_setprio
 *
 * Changes the priority of the indicated thread.  Note that if the thread
 * is in the UT_RUNNABLE state (it's runnable but not on cpu) you should
 * change the list it's waiting on so the effect of this call is
 * immediate.
 */
int
uthread_setprio(uthread_id_t id, int prio)
{
    // TODO: Since we're potentially modifying the run queue here, we
    // also need to synchronize the invoking uthreads.
    //
    // Hint: Use runq_mtx

    if (prio > UTH_MAXPRIO)
        return 0;
    if ((id < 0) || (id >= UTH_MAX_UTHREADS))
        return 0;
    uthread_t *thr = &uthreads[id];
    uthread_nopreempt_on();
    if ((thr->ut_state == UT_NO_STATE) || (thr->ut_state == UT_TRANSITION) ||
        (thr->ut_state == UT_ZOMBIE)) {
        uthread_nopreempt_off();
        return 0;
    }
    if (thr->ut_prio == prio) {
        uthread_nopreempt_off();
        return 1;
    }
    int oldprio = thr->ut_prio;
    thr->ut_prio = prio;
    pthread_mutex_lock(&runq_mtx);
    if (thr->ut_state == UT_RUNNABLE) {
        uthread_runq_requeue(thr, oldprio);
    }
    if ((prio > ut_curthr->ut_prio) && (thr->ut_state == UT_RUNNABLE)) {
        // yield if its priority is better than caller's
        uthread_yield();
    }
    pthread_mutex_unlock(&runq_mtx);
    uthread_nopreempt_off();
    return 1;
}



/* ----------- (mostly) private code -- */

void uthread_startonrunq(uthread_id_t id, int prio) {
    // TODO: Make sure no other threads can touch the runq mutex while we're
    // modifying it

    /* preemption should be off -- requirement of uthread_runq_enqueue.
     * this is, in fact, done in uthread_create() before calling uthread_startonrunq
     */

    uthread_t *thr = &uthreads[id];

    uthread_nopreempt_on();
    pthread_mutex_lock(&runq_mtx);

    thr->ut_prio = prio;
    if (thr->ut_state == UT_TRANSITION) {
        // newly created thread
        thr->ut_state = UT_RUNNABLE;
        uthread_runq_enqueue(thr);
        pthread_mutex_unlock(&runq_mtx);
        uthread_nopreempt_off();
    } else {
        PANIC("new thread not in UT_TRANSITION state");
    }
}

static void lwp_park(void);
static int runq_size;	/* number of threads on the runq */


/*
 * uthread_switch()
 *
 * This is where all the magic begins. Save the uthread's context (using getcontext).
 * This is where the uthread will resume, so use a volatile local variable to distinguish
 * between first and second return from getcontext. Put any parameters into LWP's struct,
 * so it can act on them after getting out of uthread's context. Finally, switch to
 * content of LWP using setcontext.
 */

void uthread_switch(utqueue_t *q, int saveonrq, pthread_mutex_t *m) {
    // TODO:
    // 1. Lock runq_mtx and get the current uthread's context.
    //
    // 2. Use a volatile local variable to determine whether
    // it's the first or second return from getcontext. We use the volatile
    // keyword because this prevents gcc from optimizing it away, forcing it
    // to be reread if the same thread resumes on a new processor.
    //
    // 3. Setup the thread local curlwp with the passed arguments to operate
    // on from lwp_switch (where we are no longer on the current threads
    // stack)

    // thread preeemption should be off
    assert(is_masked());

    // lock run queue
    pthread_mutex_lock(&runq_mtx);
    volatile int first = 1;

    // save the current thread's context
    uthread_getcontext(&(ut_curthr->ut_ctx));

    // this allows you to edit 'first' on the stack even after registers have been saved
    if(!first)
      return;

    first = 0;

    // update LWP fields
    curlwp->queue = q;
    curlwp->saveonrq = saveonrq;
    curlwp->pmut = m;

    // switch to context of LWP
    uthread_setcontext(&(curlwp->lwp_ctx));
}


/*
 * check whether queue is a run queue; move over to uthread_util.c later
 */
bool is_runq(utqueue_t* queue) {
    int i;
    for(i = 0; i < UTH_MAXPRIO; i++)
      if (queue == &(runq_table[i]))
        return true;

    return false;
}


/*
 * lwp_switch()
 *
 * This is where LWPs hang out when they're looking for uthreads to run. They each call this
 * just once when they start up. Within lwp_switch they immediately call getcontext to
 * save their context, which is where they resume when switched to from uthread_switch.
 * They first do operations requested by the just-switched-from uthread, then they search the
 * runq for a runnable uthread. If they don't find one, they call lwp_park where they wait until
 * released.
 */

void lwp_switch() {
    // TODO:
    // 1. Save the context of this LWP in curlwp using getcontext
    //
    // 2. Check the arguments that are set in curlwp from uthread_switch.
    // If a given argument is not null, perform the appropriate action:
    //
    //	- For curlwp->queue, enqueue ut_curthr on the given queue
    //	  (don't forget to mark it runnable if the given queue is
    //	  a run queue! you can check for this by comparing the pointer
    //	  to the addresses of one of the queues in the runq table)
    //
    //	- For curlwp->saveonrq, make the current thread runnable. This
    //	  involves modifying the appropriate run queue.
    //
    //	- For curlwp->pmut, unlock the given mutex.
    //
    // These operations are useful if you need synchronization that requires
    // doing something concerning the current uthread but only AFTER it is no
    // longer executing on its own stack.
    //
    // 3. Find a new thread to run, dequeue it and switch into its context
    // using setcontext. Don't forget to set ut_curthr (recall that this is
    // a lwp-local variable, see the declaration in uthread.c). Don't forget
    // to unlock runq_mtx when you find a thread to run.
    //
    // 4. If no runnable threads are found call lwp_park. Note that lwp_park
    // is a blocking operation. You should structure your code so that once
    // someone calls lwp_release() (e.g. utqueue_runq_enqueue), the blocked
    // lwp's can again search for a thread to run (an infinite while loop
    // will do)

    // save context of lwp
    uthread_getcontext(&(curlwp->lwp_ctx));

    // place thread into a queue; run queue should still be locked from thread_switch()
    if(curlwp->queue) {
      if(is_runq(curlwp->queue)) {
        ut_curthr->ut_state = UT_RUNNABLE;
        uthread_runq_enqueue(ut_curthr);
      }
      else {
        utqueue_enqueue(curlwp->queue, ut_curthr);
      }
    }

    // if you're to save the thread on the run queue but it's not already there
    if(curlwp->saveonrq && !curlwp->queue) {
        ut_curthr->ut_state = UT_RUNNABLE;
        uthread_runq_enqueue(ut_curthr);
    }

    /*
     * you can't start running the thread when you place it on run queue -- you have
     * to be sure you've switched out of the thread's stack s.t. two kernel threads
     * don't start running the same thread at the same time.
     */
    if(curlwp->pmut) {
      pthread_mutex_unlock(curlwp->pmut);
    }

    // find a new thread to run
    while(1) {
      int i;
      for(i = UTH_MAXPRIO; i >= 0; i--) {
        if(utqueue_empty(&runq_table[i])) {
	  continue;
        }
        else {
	  ut_curthr = utqueue_dequeue(&runq_table[i]);
	  list_link_init(&ut_curthr->ut_link);
	  runq_size--;
	  ut_curthr->ut_state = UT_ON_CPU;
          pthread_mutex_unlock(&runq_mtx);
	  uthread_setcontext(&(ut_curthr->ut_ctx));
        }
      }

      // no runnable threads, block until one becomes available
      lwp_park();
    }
}


static void uthread_start_timer(void);
/*
 * uthread_sched_init
 *
 * Setup the scheduler. This is called once from uthread_init().
 */
void
uthread_sched_init(void)
{
    int i;
    for (i=0; i<=UTH_MAXPRIO; i++) {
        utqueue_init(&runq_table[i]);
    }
    uthread_start_timer();
}

static void clock_interrupt(int);
sigset_t VTALRMmask;
__thread int masked = 1;

static inline int is_masked() {return masked;}

static void
uthread_start_timer() {
    // start the time-slice timer.
    // It's process-wide, not per LWP and is delivered to randomly chosen LWP.
    // The posix signal mask is per LWP. The uthread signal mask is per uthread.
    sigemptyset(&VTALRMmask);
    sigaddset(&VTALRMmask, SIGVTALRM);
    struct timeval interval = {0, 1}; // every .001 milliseconds
    struct itimerval timerval;
    timerval.it_value = interval;
    timerval.it_interval = interval;
    signal(SIGVTALRM, clock_interrupt);
    pthread_sigmask(SIG_BLOCK, &VTALRMmask, 0); // initially masked
    setitimer(ITIMER_VIRTUAL, &timerval, 0); // off we go!
}

static void
clock_interrupt(int sig) {
    // handler for SIGVTALRM
    assert(!is_masked());
    if (ut_curthr == 0 || ut_curthr->ut_state != UT_ON_CPU) {
        // don't want to deal with it now: either there's no running uthread to preempt on
        // this LWP, or the uthread is masking clock interrupts
        return;
    }
    assert(ut_curthr->ut_no_preempt_count == 0);
    pthread_sigmask(SIG_UNBLOCK, &VTALRMmask, 0);
    uthread_yield();
}

void uthread_nopreempt_on() {
    // mask clock interrupts for current thread; calls may be nested
    // must not refer to ut_curthr without clock interrupts masked
    pthread_sigmask(SIG_BLOCK, &VTALRMmask, 0);
    masked = 1;
    assert(ut_curthr != NULL);
    ut_curthr->ut_no_preempt_count++;
    assert(ut_curthr->ut_no_preempt_count > 0);
}

void uthread_nopreempt_off() {
    // unmask clock interrupts for current thread; since calls may be nested,
    // must keep track of whether this the "last" call to turn on clock interrupts
    assert(ut_curthr != NULL);
    assert(ut_curthr->ut_no_preempt_count > 0);
    if (--ut_curthr->ut_no_preempt_count == 0) {
        masked = 0;
        pthread_sigmask(SIG_UNBLOCK, &VTALRMmask, 0);
    }
}

static pthread_cond_t lwp_park_cond = PTHREAD_COND_INITIALIZER;
static int lwp_parked_cnt;

/*
 *
 * lwp_park()
 *
 * This is where LWPs hang out when they have nothing to do.
 */

static void lwp_park() {
    // TODO: Block the lwp on the condition variable until
    // lwp_release is called. All bookkeeping for lwp_parked_cnt
    // is done here. Note that runq_mtx is locked on entry, and
    // it should by locked on exit (recall that switching into a
    // thread in lwp_switch involves unlocking the runq mutex)
    //
    // Why shouldn't lwp_parked_cnt == lwp_cnt? What does this mean?

    lwp_parked_cnt++;

    // if all LWPs are blocked, nothing will ever wake them up
    assert(lwp_parked_cnt != lwp_cnt);

    // block the LWP until there's something in the run queue
    while(!runq_size) {
      pthread_cond_wait(&lwp_park_cond, &runq_mtx);
    }

    lwp_parked_cnt--;
}

/*
 *
 * lwp_release()
 *
 * A uthread has become runnable. Wake up an LWP to deal with it.
 */

static void lwp_release() {
    pthread_cond_signal(&lwp_park_cond);
}

void uthread_runq_enqueue(uthread_t *thr) {
    // TODO: Enqueue the uthread on its appropriate runq.
    // Remember to bump runq_size and call lwp_release if
    // runq_size was just zero (since now there's a thread to
    // run within an lwp)
    //
    // You may assume that preemption has been disabled and
    // that runq_mtx locking takes place outside of this function.

    assert(ut_curthr->ut_no_preempt_count);
    assert(thr->ut_state == UT_RUNNABLE);

    // enqueue thread on run queue
    utqueue_enqueue(&runq_table[thr->ut_prio], thr);

    runq_size++;

    if(runq_size == 1)
      lwp_release();
}

static void uthread_runq_requeue(uthread_t *thr, int oldprio) {
    // TODO: Remove thr from the run queue for oldprio priority
    // uthreads, and enqueue it on the run queue associated with
    // thr's current priority.
    //
    // You may assume that preemption has been disabled and
    // that runq_mtx locking takes place outside of this function.

    assert(ut_curthr->ut_no_preempt_count);
    assert(thr->ut_state == UT_RUNNABLE);

    /* make sure thread is on a queue; if a thread is UT_RUNNABLE but not
     * already on a queue, then this means it's recently been dequeued or
     * is about to be enqueued. In this case, don't do anything */
    if(thr->ut_link.l_prev && thr->ut_link.l_next) {
      utqueue_remove(&runq_table[oldprio], thr);
      utqueue_enqueue(&runq_table[thr->ut_prio], thr);
    }
}
