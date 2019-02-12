// vim: sw=8:ts=8:noexpandtab

#include <inttypes.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>

#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <stdarg.h>

#define BUFFER_SIZE 1024

// Enums for encapsulating request/response codes
enum request {
    HELLO,
    SET_STATION,
};

enum response {
    WELCOME,
    ANNOUNCE,
    INVALID,
};

// Macro to get the max of two numbers
#define MAX(a, b) (a > b ? a : b)

// Send a "HELLO" message to the server through TCP
void send_hello(int tcp_socket, int udpport) {
	/*
	A HELLO message contains 3 bytes.
	The first is a command indicator, and the second two specify a UDP port.
	*/
	uint8_t command = HELLO;
	// The port number is assumed to be a host int
	uint16_t port_n = htons(udpport);

	// First write the command
	if(write(tcp_socket, &command, sizeof(command)) < 0) {
		perror("write");
		exit(1);
	}
	// Then write the port number
	if(write(tcp_socket, &port_n, sizeof(port_n)) < 0) {
		perror("write");
		exit(1);
	}
}

// Send a SET_STATION command to the server through TCP
void send_set_station(int tcp_socket, int station) {
	/*
	A Set Station command contains 3 bytes.
	The first indicates the command, and the second two specify a station.
	*/
	uint8_t command = SET_STATION;
	uint16_t station_n = htons(station);

	// First write the command
	if(write(tcp_socket, &command, sizeof(command)) < 0) {
		perror("write");
		exit(1);
	}
	// Then write the station number
	if(write(tcp_socket, &station_n, sizeof(station_n)) < 0) {
		perror("write");
		exit(1);
	}
}

/*
Handle input from the user
Returns 1 if the program should exit, 0 otherwise
*/
int handle_input(char *buf, int tcp_socket, int channels) {
	// Ignore all leading spaces by advancing the buffer pointer
	while(isspace(*buf) && *buf != '\n') {
		buf++;
	}

	// If the line was just a newline character, do nothing.
	if(buf[0] == '\n' || buf[0] == '\0') {
		return 0;
	}

	// Just 'q'
	if(buf[0] == 'q' && buf[1] == '\n') {
		return 1;
	}
	// it's a number
	// Check string for nondigit characters and spaces
	char *chkbuf = buf;
	while(*chkbuf != '\n' && *chkbuf != '\0') {
		if (!isdigit(*(chkbuf++))) {
			if (isspace(*chkbuf)) {
				*chkbuf = '\0';
				break;
			} else {
				// remember to print to stderr, not stdout
				fprintf(stderr, "Invalid command.\n");
				return 0;
			}
		}
	}

	// Make the buffer into an integer and send the command accordingly
	int station = atoi(buf);
	if(station < channels) {
		send_set_station(tcp_socket, station);
	} else  {
		// if the station number was too high, don't send it
		fprintf(stderr, "Invalid station.\n");
	}


	return 0;
}

int main(int argc, char **argv) {
	/* USAGE: ./client hostname serverport udpport */
	if(argc != 4) {
	  fprintf(stderr, "Usage: %s <hostname> <serverport> <udpport>\n", argv[0]);
	  exit(1);
	}

	// get the arguments from argv
	int udpport = atoi(argv[3]);

	// Create tcp socket
	int tcp_socket;

	// this might be redundant
	if ((tcp_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
 	  perror("socket");
 	  exit(1);
	}

	// Create udp socket
	int udp_socket;

	if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
 	  perror("socket");
 	  exit(1);
	}

	// Set up and bind UDP socket

	struct addrinfo udp_hints;
	struct addrinfo *result;

	// Fill in udp_hints struct
	memset(&udp_hints, 0, sizeof(udp_hints));
	udp_hints.ai_family = AF_INET;
	udp_hints.ai_socktype = SOCK_DGRAM;

	int err;

	if((err = getaddrinfo(NULL, argv[3], &udp_hints, &result)) != 0) {
		fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(err));
		exit(1);
	}

	struct addrinfo *r;

	// Loop through results of getaddrinfo
	// Bind the udp_socket
	// Use bind because for udp you don't really care about sending info
	for (r = result; r != NULL; r = r->ai_next) {

	  // try all the interfaces in the result struct
	  if ((udp_socket = socket(r->ai_family, r->ai_socktype, r->ai_protocol)) < 0)
	    continue;

	  if (!bind(udp_socket, r->ai_addr, r->ai_addrlen))
	    break;

	  close(udp_socket);

	  if (r == NULL) {
	    fprintf(stderr, "Could not connect to %s\n", argv[1]);
	    exit(1);
	  }
	}

	freeaddrinfo(result);

	// Setup and connect the TCP port

	struct addrinfo tcp_hints;

	// Fill in tcp_hints struct
	memset(&tcp_hints, 0, sizeof(tcp_hints));
	tcp_hints.ai_family = AF_INET;
	tcp_hints.ai_socktype = SOCK_STREAM;

	if((err = getaddrinfo(argv[1], argv[2], &tcp_hints, &result)) != 0) {
		fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(err));
		exit(1);
	}

	// Loop through results of getaddrinfo
	// Connect the tcp_socket
	// Use connect because for tcp you care about sending info back and forth
	for (r = result; r != NULL; r = r->ai_next) {

	  // try all the interfaces in the result struct
	  if ((tcp_socket = socket(r->ai_family, r->ai_socktype, r->ai_protocol)) < 0)
	    continue;

	  if (connect(tcp_socket, r->ai_addr, r->ai_addrlen) >= 0)
	    break;

          close(tcp_socket);

	  if (r == NULL) {
	    fprintf(stderr, "Could not connect to %s\n", argv[1]);
	    exit(1);
	  }
	}

	freeaddrinfo(result);

	// includes stdin.
	// Set up a file descriptor set for the select() loop
	fd_set inputs;

	// set up num_fds, the number of file descriptors must be one more than
	// the maximum file number
	int num_fds = MAX(tcp_socket, udp_socket) + 1;

	// Start the connection by sending a hello
	send_hello(tcp_socket, udpport);
	// how many stations there are
	int stations = 0;

	// The select() loop
	while(1) {

	  // Zero out the fd_set
	  FD_ZERO(&inputs);

	  // add in the three things we need to look at, which are
	  // tcp socket, udp socket and stdin
	  FD_SET(tcp_socket, &inputs);
	  FD_SET(udp_socket, &inputs);
	  FD_SET(STDIN_FILENO, &inputs);

	  // store the value of select() & perform select
	  if(select(num_fds, &inputs, 0, 0, 0) < 0) {
	    fprintf(stderr, "Problem with select\n");
	    exit(1);
	  }

	  // If the TCP socket recieved something;
	  if (FD_ISSET(tcp_socket, &inputs)) {
	    uint8_t reply_type;
	    if (read(tcp_socket, &reply_type, sizeof(uint8_t)) != sizeof(uint8_t)) {
	      fprintf(stderr, "Could not get reply type\n");
	      exit(1);
	    }

	    // if we got WELCOME
	    if(reply_type == 0) {
	      // welcome the user to the station
    	      fprintf(stderr, "Welcome!\n");

	      // get number of channels
	      if (read(tcp_socket, &stations, sizeof(uint16_t)) != sizeof(uint16_t)) {
	        fprintf(stderr, "Could not get number of stations\n");
	        exit(1);
	      }

	      send_set_station(tcp_socket, 0);
	    }

	    // if we got ANNOUNCE
	    if(reply_type == 1) {
	      uint8_t song_name_size;

	      // get song name
	      if (read(tcp_socket, &song_name_size, sizeof(uint8_t)) != sizeof(uint8_t)) {
	        fprintf(stderr, "Could not get size of song name\n");
	        exit(1);
	      }

	      // create buffer to hold message
	      char buffer[song_name_size + 1];
	      if (read(tcp_socket, &buffer, song_name_size) != song_name_size) {
	        fprintf(stderr, "Could not get announcement\n");
	        exit(1);
	      }

	      // null terminate the string and pring it out
	      buffer[song_name_size] = '\0';
	      fprintf(stderr, "%s\n", buffer);
	    }

	    // if we got INVALID COMMAND
	    if(reply_type == 2) {
	      uint8_t reply_string_size;

	      // get reply
	      if (read(tcp_socket, &reply_string_size, sizeof(uint8_t)) != sizeof(uint8_t)) {
	        fprintf(stderr, "Could not get size of reply\n");
	        exit(1);
	      }

	      // create buffer to hold message
	      char buffer[reply_string_size + 1];
	      if (read(tcp_socket, &buffer, reply_string_size) != reply_string_size) {
	        fprintf(stderr, "Could not get reply\n");
	        exit(1);
	      }

	      // null terminate the string and pring it out
	      buffer[reply_string_size] = '\0';
	      fprintf(stderr, "%s\n", buffer);

	      // close connection
	     break;
	    }
 	  }

	  if (FD_ISSET(udp_socket, &inputs)) {
	    char buffer[BUFFER_SIZE];
	    int size = read(udp_socket, &buffer, BUFFER_SIZE);

	    if(write(STDOUT_FILENO, buffer, size) != size) {
	      fprintf(stderr, "UDP write\n");
	      exit(1);
	    }
	  }

	  if (FD_ISSET(STDIN_FILENO, &inputs)) {
	    char buffer[BUFFER_SIZE];
	    int size = read(STDIN_FILENO, &buffer, BUFFER_SIZE);

	    // if nothing entered
	    if(size <= 0)
	      continue;

	    // null terminate
	    buffer[size] = '\0';

	    // if it returns 1, break out of the loop
  	    if(handle_input(buffer, tcp_socket, stations))
	      break;
	  }
        }

      // Close both the file descriptors before exiting
      close(tcp_socket);
      close(udp_socket);

      return 0;
}
