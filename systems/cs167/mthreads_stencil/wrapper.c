#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int __wrap_pthread_mutex_lock(pthread_mutex_t *m) {
  int __real_pthread_mutex_lock(pthread_mutex_t *);
  int ret = __real_pthread_mutex_lock(m);
  if (ret != 0) {
    fprintf(stderr, "%s\n", strerror(ret));
    abort();
  }
  return ret;
}

int __wrap_pthread_mutex_unlock(pthread_mutex_t *m) {
    int __real_pthread_mutex_unlock(pthread_mutex_t *);
    int ret = __real_pthread_mutex_unlock(m);
    if (ret != 0) {
        fprintf(stderr, "%s\n", strerror(ret));
        abort();
    }
    return ret;
}
