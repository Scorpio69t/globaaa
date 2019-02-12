CFLAGS = -Wall -O2 -std=c99 -D_GNU_SOURCE -ggdb
CC = gcc

EXECS = client

all: $(EXECS)

client: networking.c  
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(EXECS)
