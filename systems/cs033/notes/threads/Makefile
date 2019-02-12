CFLAGS = -O2 -std=c99 -D_GNU_SOURCE -ggdb -pthread
CC = gcc

EXECS = threads_mm

all: threads_mm

threads_mm: threads_mm.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(EXECS)
