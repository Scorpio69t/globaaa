CFLAGS = -g3 -Wall -Wextra -Wconversion -Wcast-qual -Wcast-align -g
CFLAGS += -Winline -Wfloat-equal -Wnested-externs
CFLAGS += -pedantic -std=gnu99 -D_GNU_SOURCE -Werror
CC = gcc

EXECS = 33sh 33noprompt input loop norm

PROMPT = -DPROMPT

all: 33sh 33noprompt loop input norm

33sh: sh.c helpers/shell_1_helpers.c helpers/shell_2_helpers.c helpers/helpers.h jobs.c jobs.h
	$(CC) $< $(CFLAGS) $(PROMPT) helpers/shell_1_helpers.c helpers/shell_2_helpers.c jobs.c -o $@

33noprompt: sh.c helpers/shell_1_helpers.c helpers/shell_2_helpers.c jobs.c jobs.h
	$(CC) $< $(CFLAGS) helpers/shell_1_helpers.c helpers/shell_2_helpers.c jobs.c -o $@

loop: testers/loop.c
	$(CC) $< $(CFLAGS) -o $@

input: testers/input.c
	$(CC) $< $(CFLAGS) -o $@

norm: testers/norm.c
	$(CC) $< $(CFLAGS) -o $@

clean:
	rm -rf $(EXECS)
