#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "./comm.h"

#define QUEUE 1000

/* Serverside IO functions */

/* The server socket used to listen for incoming client
 * connections in the listener thread. */
int lsock;

/* The listener thread sets up lsock using bind and listen and
 * then waits for incoming client connections, calling the server's
 * client_constructor routine every time a new connection socket is
 * acquired. */
static void *listener(void (*server)(FILE *));

/* The port that the listener thread is listening on. This
 * should be the same port that clients use in getaddrinfo
 * to find the server and create a connection socket. */
static int comm_port;

pthread_t start_listener(int port, void (*server)(FILE *)) {
    // Arguments are the port number of the server and the function
    // client_constructor (from server.c).
    // TODO: Creates a thread that listens for clients (by running the
    // listener function in this file) and detaches the thread.

    // comm_port is global
    comm_port = port;

    // create thread that listens
    pthread_t thr;
    pthread_create(&thr, NULL, (void * (*)(void *)) listener, server);

    // detached threads' resources will be returned upon exit
    if(pthread_detach(thr)) {
      perror("detach");
      exit(1);
    }

    // return thread; this is just a number, a thread ID
    return thr;
}

void *listener(void (*server)(FILE *)) {
    // TODO:
    // Step 1: set up a listener socket, bind it to the appropriate
    //     port and address, and listen for connections
    // Step 2: infinitely loop accepting connections from clients, printing out
    //         a message appropriately and using the function passed in to
    //         create a client in the server. Be sure to close sockets on error
    //         conditions.
    //
    // This should be the most logic-heavy function in this source file, it
    // is responsible for creating new client threads by calling the passed
    // server function with the corresponding connection stream every time
    // a client process connects to the server

    // create socket *** taken from lecture 30 ***
    if ((lsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket");
      exit(1);
    }

    // create struct to hold addr info
    struct sockaddr_in my_addr;

    // set up addr info
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;

    // INADDR_ANY: will bind to all IPs of the machine
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(comm_port);

    // bind name to socket
    if (bind(lsock, (struct sockaddr*) &my_addr, sizeof(my_addr)) < 0) {
      perror("bind");
      close(lsock);
      exit(1);
    }

    // listen for incoming connections
    if (listen(lsock, QUEUE) < 0) {
      perror("listen");
      close(lsock);
      exit(1);
    }

    while(1) {
      int csock;
      struct sockaddr_in client_addr;
      socklen_t client_len = sizeof(client_addr);

      // you've created a connection to a client; csock is a new FD representing client
      csock = accept(lsock, (struct sockaddr*) &client_addr, &client_len);
      printf("Received connection from %s#%hu\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);

      pthread_t thr;
      if(pthread_create(&thr, NULL, (void * (*)(void *)) server, (void*) fdopen(csock, "w+"))) {
        perror("client thread creation");
        close(csock);
        exit(1);
      }

      // return client resources
      pthread_detach(thr);
    }

    close(lsock);
    return NULL;
}

/* Shuts down the i/o stream */
void comm_shutdown(FILE *cxstr) {
    if (fclose(cxstr) < 0) {
        perror("Error closing i/o stream:");
    }
}

/* Arguments are an input/output stream, response and command.
 * If there is a response to the previous command, send it through the stream to
 * the client. Receive a command from the stream and store it so that the server
 * can handle it. Returns 0 on success, and -1 if the client disconnected during
 * serving. */
int comm_serve(FILE *cxstr, char *response, char *command) {
    int rlen = strlen(response);
    // a null response must be sent, and only sent, on the first call
    // to comm_serve
    if (rlen > 0) {
        if (fputs(response, cxstr) == EOF || fputc('\n', cxstr) == EOF
                || fflush(cxstr) == EOF) {
            fprintf(stderr, "client connection terminated\n");
            return -1;
        }
    }

    if (fgets(command, BUFLEN, cxstr) == NULL) {
        fprintf(stderr, "client connection terminated\n");
        return -1;
    }

  return 0;
}

