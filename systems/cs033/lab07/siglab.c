#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>

#define BLOCK 512
#define INTERVAL 2
#define TRUE true
#define FALSE false

// Forward Declarations
int install_handler(int sig, void (*handler)(int));
void sigint_handler(int sig);
void sigtstp_handler(int sig);
void sigquit_handler(int sig);
int read_and_echo();

// global to keep track of time
time_t seconds;
bool sig_int, sig_stop, sig_quit;

/* main
 * Install the necessary signal handlers, then call read_and_echo().
 */
int main(int argc, char** argv) {
	sigset_t old;
	sigset_t full;
	sigfillset(&full);

	// Ignore signals while installing handlers
	sigprocmask(SIG_SETMASK, &full, &old);

	// Install signal handlers
	if(install_handler(SIGINT, &sigint_handler))
		perror("Warning: could not install handler for SIGINT");

	if(install_handler(SIGTSTP, &sigtstp_handler))
		perror("Warning: could not install handler for SIGTSTP");

	if(install_handler(SIGQUIT, &sigquit_handler))
		perror("Warning: could not install handler for SIGQUIT");

	// Restore signal mask to previous value
	sigprocmask(SIG_SETMASK, &old, NULL);

	read_and_echo();

	return 0;
}


/* install_handler
 * Installs a signal handler for the given signal
 * Returns 0 on success, -1 on error
 */
int install_handler(int sig, void (*handler)(int)) {
  // TODO: Use sigaction() to install a the given function
  // as a handler for the given signal.

  struct sigaction act;
  // set struct's handler to be the same as *handler
  act.sa_handler = handler;

  // set masks; this is from lecture - should zero it
  sigemptyset(&act.sa_mask);

  // set flags; I think this would be something if you were using sa_sigaction instead of sa_handler
  act.sa_flags = 0;

  if(sigaction(sig, &act, NULL) < 0)
    return -1;
  else
    return 0;
}


/* sigint_handler
 * Respond to SIGINT signal (CTRL-C)
 *
 * Argument: int sig - the integer code representing this signal
 */
void sigint_handler(int sig) {
  // set time
  seconds = time(NULL);

  // update global variables
  sig_int = TRUE;
  sig_stop = FALSE;
  sig_quit = FALSE;

  char buffer[BLOCK];
  int sz = sprintf(buffer, "%d signal received!\n", sig);

  // if write error
  if(write(STDOUT_FILENO, buffer, sz) < 0)
    exit(1);
}


/* sigtstp_handler
 * Respond to SIGTSTP signal (CTRL-Z)
 *
 * Argument: int sig - the integer code representing this signal
 */
void sigtstp_handler(int sig) {
  time_t stp_seconds = time(NULL);

  // for knocker
  if(stp_seconds - seconds < INTERVAL) {
    if(sig_int && !sig_stop && !sig_quit) {
      sig_stop = TRUE;
    }
    else {
      sig_int = FALSE;
      sig_stop = FALSE;
      sig_quit = FALSE;
    }
  }
  else {
    sig_int = FALSE;
    sig_stop = FALSE;
    sig_quit = FALSE;
  }

  char buffer[BLOCK];
  int sz = sprintf(buffer, "%d signal received!\n", sig);

  // if write error
  if(write(STDOUT_FILENO, buffer, sz) < 0)
    exit(1);
}


/* sigquit_handler
 * Catches SIGQUIT signal (CTRL-\)
 *
 * Argument: int sig - the integer code representing this signal
 */
void sigquit_handler(int sig) {
  time_t quit_seconds = time(NULL);

  if(quit_seconds - seconds < INTERVAL) {
    if(sig_int && sig_stop) {
      char buffer[BLOCK];
      int sz = sprintf(buffer, "\n");

      if(write(STDOUT_FILENO, buffer, sz) < 0)
        exit(1);

      exit(0);
    }
    else {
      sig_int = FALSE;
      sig_stop = FALSE;
      sig_quit = FALSE;
    }
  }
  else {
    sig_int = FALSE;
    sig_stop = FALSE;
    sig_quit = FALSE;
  }

  char buffer[BLOCK];
  int sz = sprintf(buffer, "%d signal received!\n", sig);

  // if write error
  if(write(STDOUT_FILENO, buffer, sz) < 0)
    exit(1);
}

/* read_and_echo
 * Read input from stdin, echo to stdout.
 * Return 0 on EOF, -1 on error
 */
int read_and_echo() {
  // TODO: Read from stdin and write to stdout
  // Use the async-signal-safe syscalls read() and write()

  char buffer[BLOCK];
  ssize_t sz;

  while((sz = read(STDIN_FILENO, buffer, BLOCK)) != 0) {
    // write error
    if(sz > 0) {
      if(write(STDOUT_FILENO, buffer, sz) < 0)
        return -1;
    }
  }

  // end of file
  if(!sz)
    return 0;
  else
    return -1;
}
