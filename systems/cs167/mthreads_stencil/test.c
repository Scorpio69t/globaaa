/*
 *   FILE: test.c
 * AUTHOR: Sean Andrew Cannella (scannell)
 *  DESCR: a simple test program for uthreads
 *   DATE: Mon Sep 23 14:11:48 2002
 *
 * Modifed considerably by Tom Doeppner in support of two-level threading
 *   DATE: Sun Jan 10, 2016
 *
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "uthread.h"
#include "uthread_mtx.h"
#include "uthread_cond.h"
#include "uthread_sched.h"

#define	NUM_THREADS 3

#define SBUFSZ 256

uthread_id_t	thr[NUM_THREADS];
uthread_mtx_t	mtx;
uthread_cond_t	cond;

/* XXX: we're using sprintf and write to emulate printf - but, we're not
 * really being as judicious as we should be about guarding write. */

int turn;

static void
tester(long a0, char *a1[])
{
  int	i = 0, ret;
  char pbuffer[SBUFSZ];

  while (i < 10)
  {
    sprintf(pbuffer, "thread %i: hello! (%i)\n", uthread_self(), i++);
    ret = write(STDOUT_FILENO, pbuffer, strlen(pbuffer));
    if (ret < 0)
    {
      perror("uthreads_test");
      /* XXX: we should really cleanup here */
      exit(1);
    }

    volatile int j, k=100000000;
    for (j=0; j<1000000; j++) {
      k/=3;
    }

    uthread_mtx_lock(&mtx);
    while (turn != a0) {
      uthread_cond_wait(&cond, &mtx);
    }
    assert(turn == a0);
    turn++;
    if (turn == NUM_THREADS)
      turn = 0;
    uthread_cond_broadcast(&cond);
    uthread_mtx_unlock(&mtx);
  }

  sprintf(pbuffer, "thread %i exiting.\n", uthread_self());
  ret = write(STDOUT_FILENO, pbuffer, strlen(pbuffer));
  if (ret < 0)
  {
    perror("uthreads_test");
    /* XXX: we should really cleanup here */
    exit(1);
  }

  uthread_exit(a0);
}

static void
Different_tester(long a0, char *a1[])
{
  uthread_yield();
  volatile int j, k=100000000;
  for (j=0; j<1000000; j++) {
    k/=3;
  }

  uthread_yield();
  for (j=0; j<1000000; j++) {
    k/=3;
  }
}

void
mainthread(long ac, char *av[])
{
  int	i;

  uthread_mtx_init(&mtx);
  uthread_cond_init(&cond);

  for (i = 0; i < NUM_THREADS; i++)
  {
    uthread_create(&thr[i], tester, i, NULL, 0);
  }
  uthread_setprio(thr[0], 2);
  uthread_id_t dthread;
  uthread_create(&dthread, Different_tester, 0, 0, 1);
  uthread_detach(dthread);


  for (i = 0; i < NUM_THREADS; i++)
  {
    char pbuffer[SBUFSZ];
    int	tmp, ret;

    uthread_join(thr[i], &tmp);

    sprintf(pbuffer, "joined with thread %i, exited %i.\n", thr[i], tmp);
    ret = write(STDOUT_FILENO, pbuffer, strlen(pbuffer));
    if (ret < 0)
    {
      perror("uthreads_test");
      return;
    }

    uthread_mtx_lock(&mtx);
    uthread_cond_signal(&cond);
    uthread_mtx_unlock(&mtx);
  }

  uthread_exit(0);

  return;
}

int
main(int argc, char *argv[]) {
	uthread_start(mainthread,  argc, argv);
	return 0;
}
