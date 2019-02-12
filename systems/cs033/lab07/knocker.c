/* This program reads command line arguments
 * and sends the corresponding signals to the specified PID */

#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SLEEP 1000

/* Main function */
int main(int argc, char** argv) {
  // TODO: Parse command line arguments and send the specified signals
  // Pause briefly between signals using sleep(), usleep(), or nanosleep()

  if(argc != 3)
    return 1;

  int pid = atoi(argv[1]);
  char* sequence = argv[2];
  int len;

  if((len = strlen(sequence)) != 3)
    return 1;

  int buffer[len];

  for(int i = 0; i < len; i++) {
    if(sequence[i] == 'c')
      buffer[i] = SIGINT;
    else if (sequence[i] == 'z')
      buffer[i] = SIGTSTP;
    else if (sequence[i] == 'q')
      buffer[i] = SIGQUIT;
    else
      buffer[i] = sequence[i];
  }

  for(int i = 0; i < len; i++) {
    kill(pid, buffer[i]);
    usleep(SLEEP);
  }

  return 0;
}
