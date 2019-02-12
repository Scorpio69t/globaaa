#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../uthread.h"
#include "../uthread_sched.h"

#define BUFFER 256

/*
 * Check whether you can create and join
 * threads correctly.
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

    uthread_exit(a0);
  }
}

int main(int ac, char **av) {

  // initialize
  uthread_init();

  // create a single thread
  uthread_id_t tid;
  uthread_create(&tid, tester, 10, NULL, 4);

  int tmp, ret;
  char pbuffer[BUFFER];

  // join
  uthread_join(tid, &tmp);

  sprintf(pbuffer, "joined with thread %i, exited %i.\n", tid, tmp);
  ret = write(STDOUT_FILENO, pbuffer, strlen(pbuffer));

  if (ret < 0) {
    perror("uthreads_test");
    return EXIT_FAILURE;
  }

  // exit
  uthread_exit(0);

  return 0;
}
