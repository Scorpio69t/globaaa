# Flags to pass to compiler 
CFLAGS = -Wall -O2 -std=c99 -D_GNU_SOURCE -ggdb
LOUD = -D EXTRA_LOUD 
CC = gcc

# remove all PDF created files
PDF = README.aux README.log README.pdf

# path to extra files you need to copy
LOWER_PATH = /course/cs033/pub/labs/makefile_lab/
COPY_FILES = lower.c lower.h

# external dependencies
LOWER = lower.c 

EXECS = quiet_repl loud_repl README.pdf

.PHONY: all clean

all: $(EXECS)

quiet_repl: repl.c upper.c  $(addprefix $(LOWER_PATH), $(LOWER))
	cp $(addprefix $(LOWER_PATH), $(COPY_FILES)) .	
	$(CC) $(CFLAGS) $^ -o $@

loud_repl: repl.c upper.c $(addprefix $(LOWER_PATH), $(LOWER))
	cp $(addprefix $(LOWER_PATH), $(COPY_FILES)) .	
	$(CC) $(CFLAGS) $(LOUD) $^ -o $@

README.pdf: README.tex
	pdflatex $< $@.pdf 

clean:
	rm -rf $(EXECS) $(PDF) $(COPY_FILES)
