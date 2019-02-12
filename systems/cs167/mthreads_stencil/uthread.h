/*
 *   FILE: uthread.h
 * AUTHOR: Peter Demoreuille
 *  DESCR: userland threads
 *   DATE: Sat Sep  8 10:56:08 2001
 *
 * Modifed considerably by Tom Doeppner in support of two-level threading
 *   DATE: Sun Jan 10, 2016
 *
 */

#ifndef __uthread_h__
#define __uthread_h__

#include <sys/types.h>
#include "uthread_ctx.h"
#include "uthread_queue.h"
#include <pthread.h>


#include "list.h"


/* -------------- defs -- */

#define UTH_MAXPRIO		7		/* max thread prio */
#define UTH_MAX_UTHREADS	512		/* max threads */
#define	UTH_STACK_SIZE		128*1024		/* stack size */

#define NOT_YET_IMPLEMENTED(msg) \
    do { \
        fprintf(stderr, "Not yet implemented at %s:%i -- %s\n", \
            __FILE__, __LINE__, (msg)); \
    } while(0);

#define	PANIC(err) \
	do { \
		fprintf(stderr, "PANIC at %s:%i -- %s\n", \
			__FILE__, __LINE__, err); \
		abort(); \
	} while(0);

#undef errno
#define	errno	(ut_curthr->ut_errno)


typedef int uthread_id_t;
typedef void(*uthread_func_t)(long, char *argv[]);



typedef enum
{
	UT_NO_STATE,			/* invalid thread state */
	UT_ON_CPU,		    	/* thread is running */
	UT_RUNNABLE,			/* thread is runnable */
	UT_WAIT,		    	/* thread is blocked */
	UT_ZOMBIE,		    	/* zombie threads eat your brains! */
    	UT_TRANSITION,      		/* not yet a thread, but will be soon */

	UT_NUM_THREAD_STATES
} uthread_state_t;

/* --------------- thread structure -- */

typedef struct uthread {
    list_link_t		ut_link;		/* link on waitqueue / scheduler */

    uthread_ctx_t	ut_ctx;			/* context */
    char		*ut_stack;	       	/* user stack */
    uthread_id_t	ut_id;			/* thread's id */
    uthread_state_t	ut_state;		/* thread state */
    int			ut_prio;	    	/* thread's priority */
    int			ut_errno;	    	/* thread's errno */
    int			ut_has_exited;		/* thread exited? */
    int			ut_exit;	    	/* thread's exit value */
    int			ut_detached;		/* thread is detached? */
    utqueue_t		ut_waiter;      	/* thread waiting to join with me */
    // int        	ut_no_preempt;  	/* preemption should be masked */
    int         	ut_no_preempt_count;    /* used for nested calls to turn off preemption */
    pthread_mutex_t 	ut_pmut;    		/* Used to ensure atomic actions on thread */
} uthread_t;


typedef struct lwp {
    pthread_t 		ptid;         		// posix thread ID of LWP
    uthread_ctx_t 	lwp_ctx;  		// context of LWP returned by getcontext: never changes after initialized:
                            			// allows uthreads to switch to underlying LWP at lwp_start.

    // the following hold parameters passed by uthread calling uthread_switch, to be passed to
    // underlying LWP at lwp_start
    utqueue_t 		*queue;       		// if non-null: queue on which calling uthread should be placed
    int 		saveonrq;           	// if true, then the current thread is to be put on the run q
    pthread_mutex_t 	*pmut;  		// if non-null, posix mutex to unlock once out of call uthread context
} lwp_t;

int lwp_cnt;

extern __thread lwp_t *curlwp;  // per-LWP reference to current LWP's private context



/* --------------- prototypes -- */

extern uthread_t uthreads[UTH_MAX_UTHREADS];
extern __thread uthread_t *ut_curthr;

void uthread_start(uthread_func_t firstFunc, long argc, char *argv[]);

int uthread_create(uthread_id_t *id, uthread_func_t func, long arg1,
		   void *arg2, int prio);
void uthread_exit(int status);
uthread_id_t uthread_self(void);

int uthread_join(uthread_id_t id, int *exit_value);
int uthread_detach(uthread_id_t id);

#endif /* __uthread_h__ */
