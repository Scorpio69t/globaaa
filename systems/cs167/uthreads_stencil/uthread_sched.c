/*
 *   FILE: uthread_sched.c
 * AUTHOR: Peter Demoreuille
 *  DESCR: scheduling wack for uthreads
 *   DATE: Mon Oct  1 00:19:51 2001
 *
 * Modified to handle time slicing by Tom Doeppner
 *   DATE: Sun Jan 10, 2016
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <signal.h>
#include <sys/time.h>

#include "uthread.h"
#include "uthread_private.h"
#include "uthread_ctx.h"
#include "uthread_queue.h"
#include "uthread_bool.h"
#include "uthread_sched.h"


/* ---------- globals -- */

int clock_count;
int taken_clock_count;

static utqueue_t 			 	runq_table[UTH_MAXPRIO + 1]; /* priority runqueues */
static int uthread_no_preempt;              	/* preemption not allowed */
static int uthread_no_preempt_count;		/* used to allow nested calls to uthread_no_preempt_on */



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
uthread_yield(void)
{
  ut_curthr->ut_state = UT_RUNNABLE;

  // place the currently running thread back in run queue
  uthread_nopreempt_on();
  utqueue_enqueue(&runq_table[ut_curthr->ut_prio], ut_curthr);
  uthread_nopreempt_off();

  // switch out of thread
  uthread_switch();
  uthread_nopreempt_reset();
  return;
}



/*
 * uthread_block
 *
 * Put the current thread to sleep, pending an appropriate call to
 * uthread_wake().
 */
void
uthread_block(void)
{
  // change state to make it wait
  ut_curthr->ut_state = UT_WAIT;

  // switch out of thread
  uthread_switch();
  uthread_nopreempt_reset();
  return;
}


/*
 * uthread_wake
 *
 * Wakes up the supplied thread (schedules it to be run again).  The
 * thread may already be runnable or (well, if uthreads allowed for
 * multiple cpus) already on cpu, so make sure to only mess with it if
 * it is actually in a wait state. Note that if the target thread has
 * a higher priority than the caller does, the caller should yield.
 */
void
uthread_wake(uthread_t *uthr)
{
  // only wake up waiting threads
  if(uthr->ut_state == UT_WAIT) {

    uthr->ut_state = UT_RUNNABLE;

    // add back to run queue
    uthread_nopreempt_on();
    utqueue_enqueue(&runq_table[uthr->ut_prio], uthr);
    uthread_nopreempt_off();

    // yield if target thread's prio > ut_curthr's
    if(uthr->ut_prio > ut_curthr->ut_prio && (ut_curthr->ut_state == UT_ON_CPU)) {
      uthread_yield();
    }
  }

  return;
}


/*
 * uthread_setprio
 *
 * Changes the priority of the indicated thread.  Note that if the thread
 * is in the UT_RUNNABLE state (it's runnable but not on cpu) you should
 * change the list it's waiting on so the effect of this call is
 * immediate. Yield to the target thread if its priority is higher than
 * the caller's.
 */
int
uthread_setprio(uthread_id_t id, int prio)
{
  uthread_t* uid = &uthreads[id];

  // if thread is already runnable in a queue, change its queue
  if(uid->ut_state == UT_RUNNABLE && (uid->ut_link.l_prev || uid->ut_link.l_next)) {
    uthread_nopreempt_on();
    utqueue_remove(&runq_table[uid->ut_prio], uid);
    uthread_nopreempt_off();

    list_link_init(&uid->ut_link);
  }

  // change thread's priority
  uid->ut_prio = prio;

  // add to runnable queue
  uthread_nopreempt_on();
  utqueue_enqueue(&(runq_table[uid->ut_prio]), uid);
  uthread_nopreempt_off();

  // yield if uid has a greater priority than ut_curthr
  if(uid->ut_prio > ut_curthr->ut_prio) {
    uthread_yield();
  }

  return 0;
}

/*
 * uthread_no_preempt_on
 *
 * Disable preemption. Uses a global mask rather than making sys calls.
 */
void uthread_nopreempt_on(void) {
  uthread_no_preempt_count++;
  uthread_no_preempt = 1;
}

void uthread_nopreempt_off(void) {
  if (--uthread_no_preempt_count == 0)
    uthread_no_preempt = 0;
  assert(uthread_no_preempt_count >= 0);
}



/* ----------- private code -- */


/*
 * uthread_switch()
 *
 * This is where all the magic is.  Wait until there is a runnable thread, and
 * then switch to it using uthread_swapcontext().  Make sure you pick the
 * highest priority runnable thread to switch to. Also don't forget to take
 * care of setting the ON_CPU thread state and the current thread. Note that
 * it is okay to switch back to the calling thread if it is the highest
 * priority runnable thread.
 *
 * Every time uthread_switch() is called, uthread_idle() should be called at
 * least once.  In addition, when there are no runnable threads, you should
 * repeatedly call uthread_idle() until there are runnable threads.  Threads
 * with numerically higher priorities run first. For example, a thread with
 * priority 8 will run before one with priority 3.
 * */
void
uthread_switch() {
  int i = 0;

  while(!i) {

    /* call to uthread_idle() if there is nothing in the run_queue;
     * as mentioned on piazza, and since I'm enqueue'ing the current thread,
     * this should never run more than once in uthreads */
    uthread_idle();

    // if you're switching from a waiting thread, check all queues; otherwise check only those w/ >= prio
    int cur_prio = ut_curthr->ut_state == UT_WAIT ? 0 : ut_curthr->ut_prio;

    // find a runnable thread in the run queues; will switch back to current thread if highest prio
    for(i = UTH_MAXPRIO; i >= cur_prio; i--) {
      uthread_nopreempt_on();
      if(utqueue_empty(&runq_table[i])) {
        uthread_nopreempt_off();
        continue;
      }
      else {
        // dequeue a thread from the run table
        uthread_t* next_thread = utqueue_dequeue(&runq_table[i]);
        uthread_nopreempt_off();
        list_link_init(&next_thread->ut_link);

        // update cur_thread
        uthread_t* old_cur = ut_curthr;
        ut_curthr = next_thread;
        ut_curthr->ut_state = UT_ON_CPU;

        // swap contexts
        uthread_swapcontext(&(old_cur->ut_ctx), &(next_thread->ut_ctx));
        return;
      }
    }
  }
}

/*
 * Initialize all the run queues in runq_table
 */
void
init_runqueues(void) {
  // initialize run queues
  int i = 0;
  for( ; i <= UTH_MAXPRIO; i++) {
    utqueue_init(&runq_table[i]);
  }
}

// set up globals for time slicing
struct sigaction timesliceact;
struct timeval interval = {0, 1};
struct itimerval timerval;
static sigset_t VTALRMmask;
static void clock_interrupt(int);

void uthread_start_timer(void);

/*
 * uthread_sched_init
 *
 * Setup the scheduler. This is called once from uthread_init().
 * This also kicks off the time-slice timer.
 */
void
uthread_sched_init(void) {
  // set up signal mask
  sigemptyset(&VTALRMmask);
  sigaddset(&VTALRMmask, SIGVTALRM);

  // set up timeslicer
  timesliceact.sa_handler = clock_interrupt;
  timesliceact.sa_mask = VTALRMmask;
  timesliceact.sa_flags = SA_RESTART;

  // set up timer
  timerval.it_value = interval;
  timerval.it_interval = interval;

  // start timer
  uthread_start_timer();
}

/*
 * uthread_start_timer
 *
 * Start up the time-slice clock, which uses ITIMER_VIRTUAL and SIGVTALRM.
 * For test purposes, the time-slice interval is as small as possible
 */
void
uthread_start_timer(void) {
  sigaction(SIGVTALRM, &timesliceact, 0);
  setitimer(ITIMER_VIRTUAL, &timerval, 0);
}

/*
 * clock_interrupt
 *
 * At each clock interrupt (SIGVTALRM), call uthread_yield if preemption is not masked.
 * When implementing this function it is important to understand that uthreads
 * execute within pthreads. Make sure you are clear on what constructs are
 * pthread-specific, and what constructs are uthread-specific.
 */
static void
clock_interrupt(int sig) {
  clock_count++;
  if(!uthread_no_preempt) {
    taken_clock_count++;
    sigprocmask(SIG_UNBLOCK, &VTALRMmask, 0);
    uthread_yield();
  }
}

/*
 * uthread_nopreempt_reset
 *
 * Allow preemption, regardless of the nesting level.
 */
void uthread_nopreempt_reset(void) {
  uthread_no_preempt = 0;
}
