CFLAGS = -g3 -Wall -Wextra -Wconversion -Wcast-qual -Wcast-align -g
CFLAGS += -Winline -Wfloat-equal -Wnested-externs
CFLAGS += -pedantic -std=gnu99 -Werror
CC = gcc

EXECS = 33sh 33noprompt

PROMPT = -DPROMPT

all: 33sh 33noprompt

33sh: sh.c helpers.c helpers.h
	$(CC) $< $(CFLAGS) $(PROMPT) helpers.c -o $@

33noprompt: sh.c helpers.c helpers.h
	$(CC) $< $(CFLAGS) helpers.c -o $@

clean:
	rm -rf $(EXECS)
