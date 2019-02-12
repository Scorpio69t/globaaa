/*
 *   FILE: uthread_cond.c
 * AUTHOR: Peter Demoreuille
 *  DESCR: uthreads condition variables
 *   DATE: Mon Oct  1 01:59:37 2001
 *
 * Modifed considerably by Tom Doeppner in support of two-level threading
 *   DATE: Sun Jan 10, 2016
 *
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "uthread.h"
#include "uthread_mtx.h"
#include "uthread_cond.h"
#include "uthread_queue.h"
#include "uthread_sched.h"
#include "uthread_private.h"


/*
 * uthread_cond_init
 *
 * initialize the given condition variable
 */
void
uthread_cond_init(uthread_cond_t *cond)
{
	utqueue_init(&cond->uc_waiters);
	pthread_mutex_init(&cond->pmut, 0);
}


/*
 * uthread_cond_wait
 *
 * Should behave just like a stripped down version of pthread_cond_wait.
 * Block on the given condition variable.  The caller should lock the
 * mutex and it should be locked again on return.
 */
void
uthread_cond_wait(uthread_cond_t *cond, uthread_mtx_t *mtx)
{
	// TODO:
        // 1. Synchronize access to the condition variable struct using its pmut
        // member. This guarantees that the unlock (mtx, arg 2) and sleep operation
        // happen as one atomic step.
	//
	// 2. Ensure that the calling uthread is put on the condition
	// variable's waiters queue after it has switched contexts. The
	// potential problem is the same as it is for
	// uthread_mtx_lock/uthread_mtx_unlock, only now we're dealing
	// with uthread_cond_wait/uthread_cond_broadcast (see uthread_mtx.c
	// for more detailed explanation)

	uthread_nopreempt_on();
        // lock here to ensure unlock and queue are atomic
	pthread_mutex_lock(&cond->pmut);
	uthread_mtx_unlock(mtx);
	ut_curthr->ut_state = UT_WAIT;
	// TODO: Call uthread_switch
	uthread_switch(&cond->uc_waiters, 0, &cond->pmut);
	assert(ut_curthr->ut_link.l_next == NULL);
	uthread_mtx_lock(mtx);
	assert(ut_curthr->ut_link.l_next == NULL);
	uthread_nopreempt_off();
}


/*
 * uthread_cond_broadcast
 *
 * Wakeup all the threads waiting on this condition variable.
 * Note there may be no threads waiting.
 */
void
uthread_cond_broadcast(uthread_cond_t *cond)
{
	// TODO: Synchronize access using cond->pmut

	uthread_nopreempt_on();
	pthread_mutex_lock(&cond->pmut);
	while(!utqueue_empty(&cond->uc_waiters)) {
          uthread_t* next = utqueue_dequeue(&cond->uc_waiters);
	  pthread_mutex_unlock(&cond->pmut);
	  uthread_wake(next);
	  pthread_mutex_lock(&cond->pmut);
	}
	pthread_mutex_unlock(&cond->pmut);
	uthread_nopreempt_off();
}



/*
 * uthread_cond_signal
 *
 * wakeup just one thread waiting on the condition variable.
 * Note there may be no threads waiting.
 */
void
uthread_cond_signal(uthread_cond_t *cond)
{
	// TODO: Synchronize again using cond->pmut

	uthread_nopreempt_on();
	pthread_mutex_lock(&cond->pmut);
	uthread_t* next = utqueue_dequeue(&cond->uc_waiters);
	if (next != NULL) {
	  pthread_mutex_unlock(&cond->pmut);
	  uthread_wake(next);
        }
	else {
          pthread_mutex_unlock(&cond->pmut);
	}

	uthread_nopreempt_off();
}
