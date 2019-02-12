/// vim: noexpandtab
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "./db.h"
#include "./comm.h"
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#ifdef __APPLE__
#include "pthread_OSX.h"
#endif

#define BUF_SIZE 1024
#define STRING_END 1

/* Controls when the clients in the client list should be stopped and let go. */
typedef struct client_control {
    pthread_mutex_t go_mutex;
    pthread_cond_t go;
    int stopped;
} client_control_t;

/* Use the variables in this struct to synchronize your main thread with client
 * threads, note that client threads must have terminated before you clean up
 * the database. */
typedef struct server_control {
    pthread_mutex_t server_mutex;
    pthread_cond_t server_cond;
    int num_client_threads;
} server_control_t;

/* The encapsulation of a client thread, i.e., the thread that handles
 * commands from clients. */
typedef struct client {
    pthread_t thread;
    FILE *cxstr;    // File stream for input and output

    // for client list
    struct client *prev;
    struct client *next;
} client_t;

/*
 * The encapsulation of a thread that handles signals sent to the server.
 * When SIGINT is sent to the server all client threads should be destroyed. */
typedef struct sig_handler {
    sigset_t set;
    pthread_t thread;
} sig_handler_t;


client_t *thread_list_head;
pthread_mutex_t thread_list_mutex = PTHREAD_MUTEX_INITIALIZER;
client_control_t client_cont = {.go = PTHREAD_COND_INITIALIZER, .go_mutex = PTHREAD_MUTEX_INITIALIZER};
server_control_t server_cont = {.server_cond = PTHREAD_COND_INITIALIZER, .server_mutex = PTHREAD_MUTEX_INITIALIZER};

void *run_client(void *arg);
void *monitor_signal(void *arg);
void thread_cleanup(void *arg);


void client_control_wait() {
    // TODO: Block the calling thread until the main thread calls
    // client_control_release(). See the client_control_t struct.
    // Called by client threads to wait until progress is permitted

    while(client_cont.stopped) {
      pthread_cond_wait(&client_cont.go, &client_cont.go_mutex);
    }
}

void client_control_stop() {
    // TODO: Ensure that the next time client threads call
    // client_control_wait() at the top of the event loop
    // in run_client, they will block.
    // Called by main thread to stop client threads

    client_cont.stopped = 1;
    pthread_cond_broadcast(&client_cont.go);
}

void client_control_release() {
    // TODO: Allow clients that are blocked within client_control_wait()
    // to continue. See the client_control_t struct.
    // Called by main thread to resume client threads

    client_cont.stopped = 0;
    pthread_cond_broadcast(&client_cont.go);
}

void client_constructor(FILE *cxstr) {
    // Called by listener (in comm.c) to create a new client thread
    // You should create a new client_t struct here and initialize ALL
    // of its fields. Remember that these initializations should be
    // error-checked.
    //
    // TODO:
    // Step 1: Allocate memory for a new client and set its connection stream
    // as given by the input argument.
    // Step 2: Create the new client thread running the 'run_client' routine.

    // malloc space for new client
    client_t* c = malloc(sizeof(client_t));

    // initialize fields in the struct
    c->cxstr = cxstr;
    c->prev = NULL;
    c->next = NULL;
    pthread_create(&(c->thread), NULL, run_client, (void*) c);
    pthread_detach(c->thread);
}

void client_destructor(client_t *client) {
    // TODO: Free all resources associated with a client.
    // Whatever was malloc'd in client_constructor should
    // be free'd here!
    free(client);
}

void *run_client(void *arg) {
    // Code executed by a client thread
    // TODO:
    // Step 1: make sure that the server is still accepting clients.
    // Step 2: add client to the client list and push thread_cleanup to remove
    //       it if thread is cancelled.
    // Step 3: loop comm_serve (in comm.c) to receive commands and output
    //       responses, use database to handle commands. Note that
    //       the client may terminate the connection at any moment,
    //       in which case reading/writing to the connection stream
    //       on the server side will send this process a SIGPIPE.
    //       You must ensure that the server doesn't crash when this
    //       happens!
    // Step 4: when the client is done sending commands, exit thread cleanly
    //
    // Keep the signal handler thread in mind when writing this function!

    // add client to list; make thread-safe
    pthread_mutex_lock(&thread_list_mutex);
    if(!thread_list_head) {
      ((client_t*) arg)->next = thread_list_head;
      thread_list_head = arg;
    }
    else {
      ((client_t*) arg)->next = thread_list_head;
      thread_list_head->prev = arg;
      thread_list_head = arg;
    }
    pthread_mutex_unlock(&thread_list_mutex);

    // update # of threads
    server_cont.num_client_threads += 1;

    char com[BUFLEN];
    char res[BUFLEN];

    // set handler for pthread_cancel()
    pthread_cleanup_push(thread_cleanup, arg);

    while(1) {

      client_control_wait();
      pthread_mutex_unlock(&client_cont.go_mutex);

      if(comm_serve(((client_t*) arg)->cxstr, res, com) == -1)
        break;
      else {
        interpret_command(com, res, BUFLEN);
      }
    }

    pthread_cleanup_pop(NULL);
    thread_cleanup(arg);
    return NULL;
}

void delete_all() {
    // TODO: Cancel every thread in the client thread list with the
    // pthread_cancel function. Think about ALL
    // contexts from which this function may be called when implementing
    // it!

    // assume you cancel from head-to-toe
    while(thread_list_head) {
      // update head of list
      client_t* current_t = thread_list_head;
      thread_list_head = thread_list_head->next;

      server_cont.num_client_threads -= 1;

      // call cleanup on thread
      pthread_cancel(current_t->thread);
    }
}

void remove_from_list(client_t* arg) {
    // Remove client object from the list; lock the list so
    // you can't add things to the list when you're trying to
    // remove something.

    pthread_mutex_lock(&thread_list_mutex);

    // nothing in list
    if(!thread_list_head) {
      pthread_mutex_unlock(&thread_list_mutex);
      return;
    }

    if(thread_list_head == arg)
      thread_list_head = thread_list_head->next;
    else {

      // ptr to traverse list
      client_t* ptr = thread_list_head->next;

      while(ptr) {
        if(ptr == arg && !ptr->next) {
          ptr->prev->next = ptr->next;
	  break;
	}
	else if(ptr == arg) {
          ptr->prev->next = ptr->next;
	  ptr->next->prev = ptr->prev;
	  break;
	}
	else
	  ptr = ptr->next;
      }
    }

    pthread_mutex_unlock(&thread_list_mutex);
}

void thread_cleanup(void *arg) {
    // Cleanup routine for client threads, called on cancels and exit.
    //
    // TODO: Remove the client object from thread list
    // and call client_destructor. This function must be thread safe!
    // The client must be in the list before this routine is ever run.

    server_cont.num_client_threads -= 1;
    pthread_cond_broadcast(&server_cont.server_cond);

    // find client object in list and remove it
    remove_from_list((client_t*) arg);

    // free resources
    client_destructor(arg);
}

void *monitor_signal(void *arg) {
    // Code executed by the signal handler thread. For the purpose of this
    // assignment, there are two reasonable ways to implement this.
    // The one you choose will depend on logic in sig_handler_constructor.
    // 'man 7 signal' and 'man sigwait' are both helpful for making this
    // decision. One way or another, all of the server's client threads
    // should terminate on SIGINT. The server (this includes the listener
    // thread) should not terminate on CTRL+C!
    //
    // TODO: Wait for a SIGINT to be sent to the server process, cancel
    // all client threads when one arrives.

    int sig;

    while(1) {
      if(!sigwait(&(((sig_handler_t*) arg)->set), &sig)) {
	char msg[BUF_SIZE];
        int sz = sprintf(msg, "SIGINT received, destroying all clients...\n");
	if(write(STDERR_FILENO, msg, (size_t) sz) < 0)
          exit(1);

	delete_all();
      }
    }

    return NULL;
}

sig_handler_t *sig_handler_constructor() {
    // TODO: Create a thread to handle SIGINT. The thread that this function
    // creates should be the ONLY thread that ever responds to SIGINT.

    // create a sig_handler struct
    sig_handler_t* sh = malloc(sizeof(sig_handler_t));
    memset(sh, 0, sizeof(sig_handler_t));

    sigemptyset(&(sh->set));
    sigaddset(&(sh->set), SIGINT);

    // this will block all signals in &set; all threads created
    // after this will reflect this change
    pthread_sigmask(SIG_BLOCK, &(sh->set), 0);

    pthread_create(&(sh->thread), NULL, monitor_signal, (void*) sh);
    pthread_detach(sh->thread);

    return sh;
}

void sig_handler_destructor(sig_handler_t *sighandler) {
    // Free any resources allocated in sig_handler_constructor.
    free(sighandler);
}

/*
 * The arguments to the server should be the port number.
 * TODO:
 * Step 1: set up sig handler
 * Step 2: start a listener thread for clients (use start_listener in comm.c)
 * Step 3: loop for command line input and handle accordingly until EOF
 * Step 4: cancel listener, destroy signal handler, delete all clients and exit.
 *
 * You should ensure that the thread list is empty before termination if the
 * server receives an EOF. Think carefully about what happens in a call to
 * delete_all() and ensure that there is no way for a thread to add itself to
 * the thread list after the server's final delete_all(). */
int main(int argc, char *argv[]) {

  if(argc != 2) {
    printf("Must specify port number\n");
    exit(1);
  }

  /* STEP 1 */
  sig_handler_t* handler = sig_handler_constructor();

  /* STEP 2 */
  int port = atoi(argv[1]);
  pthread_t listen = start_listener(port, client_constructor);

  /* STEP 3 */
  char buffer[BUF_SIZE];
  size_t sz;
  while((sz = read(STDIN_FILENO, buffer, BUF_SIZE)) != 0) {

    switch(buffer[0]) {

      case 'p':
        if(sz > 2) {
	  buffer[sz - 1] = '\0';
          db_print(&buffer[2]);
	} else
	  db_print(NULL);
	break;

      case 'g':
	client_control_release();
	break;

      case 's':
	client_control_stop();
	break;

      default:
	continue;
    }
  }

  /* STEP 4 */
  pthread_cancel(listen);
  sig_handler_destructor(handler);
  delete_all();

  while(server_cont.num_client_threads > 0)
    pthread_cond_wait(&server_cont.server_cond, &server_cont.server_mutex);
  pthread_mutex_unlock(&server_cont.server_mutex);

  db_cleanup();
}
