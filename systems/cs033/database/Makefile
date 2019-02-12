CFLAGS = -Wall -O2 -std=c99 -D_GNU_SOURCE -ggdb -pthread
CC = gcc

EXECS = server client

all: server client

server: server.c comm.c db.c
	$(CC) $(CFLAGS) $^ -o $@

client: client.c comm.c db.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(EXECS) *.dSYM
