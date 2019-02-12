#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../uthread.h"
#include "../uthread_sched.h"

#define BUFFER 256

/*
 * Check whether you can initialize uthreads and then
 * exit. This is really just a test to see if the reaper
 * can successfully handle the first_thread, which should
 * have priority 7.
 */

/* taken from test.c */
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

  // exit
  uthread_exit(0);

  return 0;
}
