/* Dec 25th, 2017: Daven Farnham
 *
 * run basic matrix multiplication on square matrices. running
 * 'time ./threads_mm 0' will run substantially slower than 'time ./threads_mm 2000'
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ROWS 2000
#define COLS 2000

void* thread_mm(void* i);
void print_matrix();
void finish_multiplication(int i);
void initialize();

/* should be shared memory between threads */
int A[ROWS][COLS];
int B[ROWS][COLS];
int C[ROWS][COLS];

int main(int argc, char** argv) {

  /* check command line args */
  if(argc != 2 || atoi(argv[1]) < 0 || atoi(argv[1]) > 2000) {
    printf("./thread_mm [1 <= # of threads <= %d]\n", ROWS);
    return 1;
  }

  /* create matrices A and B */
  initialize();

  int num_threads = atoi(argv[1]);
  pthread_t threads[num_threads];

  /* create threads */
  for(int i = 0; i < num_threads; i++) {
    if(pthread_create(&threads[i], NULL, thread_mm, (void *) i)) {
      char buffer[256];
      sprintf(buffer, "Problem creating thread: %d\n", i);
      return 1;
    }
  }

  /* join all threads -- once we do, we know we can print C */
  for(int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], 0);
  }

  /* if we didn't create enough threads for matrix, finish in parent thread */
  if(num_threads < ROWS) {
    finish_multiplication(num_threads);
  }

  // print_matrix(); /* check */
  return 0;
}

/* function passed to threads */
void* thread_mm(void* i) {
  int ith = (int) i;

  for(int j = 0; j < COLS; j++) {
    int sum = 0;

    for(int k = 0; k < COLS; k++) {
      sum += A[ith][k] * B[k][j];
    }

    C[j][ith] = sum;
  }

  pthread_exit(0);
}

void print_matrix() {
  for(int i = 0; i < COLS; i++) {
    for(int j = 0; j < ROWS; j++) {
      printf("%d ", C[j][i]);
    }
    printf("\n");
  }
}

void finish_multiplication(int i) {
  for(i; i < ROWS; i++) {
    for(int j = 0; j < COLS; j++) {
      for(int k = 0; k < COLS; k++) {
	C[j][i] += A[i][k] * B[k][j];
      }
    }
  }
}

void initialize() {
  for(int i = 0; i < ROWS; i++) {
    for(int k = 0; k < COLS; k++) {
      A[i][k] = i * COLS + k;
      B[i][k] = i * COLS + k;
    }
  }
}
