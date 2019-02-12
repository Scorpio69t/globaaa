CC = gcc
CFLAGS = -Wall -Werror -std=gnu99

.PHONY: all clean
all: mmapcopy

mmapcopy: mmapcopy.c
	$(CC) $(CFLAGS) $^ -g -o $@

clean:
	rm -rf mmapcopy
