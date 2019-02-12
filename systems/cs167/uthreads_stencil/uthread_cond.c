/*
 *   FILE: uthread_cond.c
 * AUTHOR: Peter Demoreuille
 *  DESCR: uthreads condition variables
 *   DATE: Mon Oct  1 01:59:37 2001
 *
 *
 * Modified to handle time slicing by Tom Doeppner
 *   DATE: Sun Jan 10, 2016
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "uthread.h"
#include "uthread_mtx.h"
#include "uthread_cond.h"
#include "uthread_queue.h"
#include "uthread_sched.h"


/*
 * uthread_cond_init
 *
 * initialize the given condition variable
 */
void
uthread_cond_init(uthread_cond_t *cond) {
  utqueue_init(&cond->uc_waiters);
}


/*
 * uthread_cond_wait
 *
 * Should behave just like a stripped down version of pthread_cond_wait.
 * Block on the given condition variable.  The caller should lock the
 * mutex and it should be locked again after the broadcast.
 * Mask preemption to ensure atomicity.
 */
void
uthread_cond_wait(uthread_cond_t *cond, uthread_mtx_t *mtx)
{
  uthread_nopreempt_on();
  uthread_mtx_unlock(mtx);

  if(cond) {
    utqueue_enqueue(&cond->uc_waiters, ut_curthr);
    uthread_block();
  }

  uthread_mtx_lock(mtx);
}


/*
 * uthread_cond_broadcast
 *
 * Wakeup all the threads waiting on this condition variable.
 * Note there may be no threads waiting.
 * Mask preemption to protect wait queue.
 */
void
uthread_cond_broadcast(uthread_cond_t *cond)
{
  uthread_nopreempt_on();

  // wake up all threads
  while(!utqueue_empty(&cond->uc_waiters)) {
    uthread_t* deq = utqueue_dequeue(&cond->uc_waiters);
    list_link_init(&deq->ut_link);
    uthread_wake(deq);
  }

  uthread_nopreempt_off();
}



/*
 * uthread_cond_signal
 *
 * wakeup just one thread waiting on the condition variable.
 * Note there may be no threads waiting.
 * Mask preemption to protect wait queue.
 */
void
uthread_cond_signal(uthread_cond_t *cond)
{
  // mask preemption
  uthread_nopreempt_on();

  // try to dequeue from cond->uc_waiters
  if(utqueue_empty(&cond->uc_waiters)) {
    uthread_nopreempt_off();
    return;
  }

  // remove from the queue
  uthread_t* deq = utqueue_dequeue(&cond->uc_waiters);
  uthread_nopreempt_off();
  list_link_init(&deq->ut_link);

  // wake up thread
  uthread_wake(deq);
}
