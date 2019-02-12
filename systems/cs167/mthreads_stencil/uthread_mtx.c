/*
 *   FILE: uthread_mtx.c
 * AUTHOR: Peter Demoreuille
 *  DESCR: userland mutexes
 *   DATE: Sat Sep  8 12:40:00 2001
 *
 * Modifed considerably by Tom Doeppner in support of two-level threading
 *   DATE: Sun Jan 10, 2016
 *
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "uthread.h"
#include "uthread_mtx.h"
#include "uthread_sched.h"
#include "uthread_private.h"
#include <unistd.h>



/*
 * uthread_mtx_init
 *
 * Initialize the fields of the specified mutex.
 */
void
uthread_mtx_init(uthread_mtx_t *mtx)
{
	mtx->m_owner = NULL;
	utqueue_init(&mtx->m_waiters);
	pthread_mutex_init(&mtx->m_pmut, 0);
}



/*
 * uthread_mtx_lock
 *
 * Lock the mutex.  This call will block if it's already locked.  When the
 * thread wakes up from waiting, it should own the mutex (see _unlock()).
 */
void
uthread_mtx_lock(uthread_mtx_t *mtx)
{
	// TODO:
	// Recall that uthread_mtx_lock can be called by many uthreads
	// running on different processors at the same time. For this
	// reason you'll need to:
	//
	// 1. Synchronize access to the mtx using its m_pmut field.
	//
	// 2. Ensure that, if the mutex is already locked, the current
	// thread is not queued and that mtx->m_pmut is not unlocked until
	// the current thread has switched contexts.
	//
	// Suppose instead we first enqueued the running thread, unlocked
	// the mtx->m_pmut, and then switched contexts. This leaves a window
	// for another thread to call uthread_mtx_unlock, dequeue the thread
	// was just about to switch contexts, and wake it up. This results in
	// a situation where we have the same thread running no two processors
	// at once. No good.

	uthread_nopreempt_on();

        // lock the mutex's mutex; prevent multiple processors from getting access
        pthread_mutex_lock(&mtx->m_pmut);

	if (mtx->m_owner == NULL) {
		mtx->m_owner = ut_curthr;
                pthread_mutex_unlock(&mtx->m_pmut);
		uthread_nopreempt_off();
	}
	else {

		// trying to unlock a mutex it doesn't yet own -- who owns mutex to this
		uthread_state_t old = ut_curthr->ut_state;
		ut_curthr->ut_state = UT_WAIT;

		/* should unlock mtx->m_pmut in uthread_switch(); will only change state to UT_RUNNABLE
		 * if the queue you're switching to is a run queue */
		uthread_switch(&mtx->m_waiters, 0, &mtx->m_pmut);

		ut_curthr->ut_state = old == UT_ZOMBIE ? UT_ZOMBIE : ut_curthr->ut_state;
		assert(ut_curthr->ut_link.l_next == NULL);
		uthread_nopreempt_off();
	}
}


/*
 * uthread_mtx_trylock
 *
 * Try to lock the mutex, return 1 if we get the lock, 0 otherwise.
 * This call should not block.
 */
int
uthread_mtx_trylock(uthread_mtx_t *mtx)
{
	// TODO: Synchronize for uthreads using mtx->m_pmut.

	uthread_nopreempt_on();
        pthread_mutex_lock(&mtx->m_pmut);
	if (mtx->m_owner == NULL) {
		mtx->m_owner = ut_curthr;
                pthread_mutex_unlock(&mtx->m_pmut);
		uthread_nopreempt_off();
		return 1;
	} else {
                pthread_mutex_unlock(&mtx->m_pmut);
		uthread_nopreempt_off();
		return 0;
	}
}


/*
 * uthread_mtx_unlock
 *
 * Unlock the mutex.  If there are people waiting to get this mutex,
 * explicitly hand off the ownership of the lock to a waiting thread and
 * then wake that thread.
 */
void
uthread_mtx_unlock(uthread_mtx_t *mtx)
{
	// TODO: Synchronize for uthreads with mtx->m_pmut

	uthread_nopreempt_on();
        pthread_mutex_lock(&mtx->m_pmut);
	if (utqueue_empty(&mtx->m_waiters)) {
		mtx->m_owner = NULL;
                pthread_mutex_unlock(&mtx->m_pmut);
	} else {
		uthread_t *next = utqueue_dequeue(&mtx->m_waiters);
		mtx->m_owner = next;
                pthread_mutex_unlock(&mtx->m_pmut);
		uthread_wake(next);
	}
	uthread_nopreempt_off();
}
