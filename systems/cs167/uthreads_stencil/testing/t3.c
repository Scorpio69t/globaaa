#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../uthread.h"
#include "../uthread_sched.h"

#define NUM_THREADS 7
#define BUFFER 256

// array of threads
uthread_id_t    thr[NUM_THREADS];

/*
 * Similar to t2.c but with more threads
 */

void tester(long a0, char *a1[]) {
  int i = 0, ret;
  char pbuffer[BUFFER];

  while (i < 10) {

    sprintf(pbuffer, "thread %i: hello! (%i)\n", uthread_self(), i++);
    ret = write(STDOUT_FILENO, pbuffer, strlen(pbuffer));
    if (ret < 0) {
        perror("uthreads_test");
        /* XXX: we should really cleanup here */
        exit(1);
    }

    volatile int j, k=100000000;
    for (j=0; j<1000000; j++) {
      k/=3;
    }

    sprintf(pbuffer, "thread %i exiting.\n", uthread_self());
    ret = write(STDOUT_FILENO, pbuffer, strlen(pbuffer));
    if (ret < 0) {
        perror("uthreads_test");
        /* XXX: we should really cleanup here */
        exit(1);
    }
  }

  uthread_exit(a0);
}

int main(int ac, char **av) {

  // initialize everything
  uthread_init();

  int i = 0;

  for( ; i < NUM_THREADS; i++) {
    // create 7 threads
    uthread_create(&thr[i], tester, i, NULL, 0);
  }

  // change priority of first thread
  uthread_setprio(thr[0], 2);

  for (i = 0; i < NUM_THREADS; i++) {
    char pbuffer[BUFFER];
    int tmp, ret;

    uthread_join(thr[i], &tmp);

    sprintf(pbuffer, "joined with thread %i, exited %i.\n", thr[i], tmp);
    ret = write(STDOUT_FILENO, pbuffer, strlen(pbuffer));

    if (ret < 0) {
      perror("uthreads_test");
      return EXIT_FAILURE;
    }
  }

  uthread_exit(0);

  return 0;
}
