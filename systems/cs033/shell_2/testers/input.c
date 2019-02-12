/*
 * read input from the user
 */

#include <stdio.h>
#include <unistd.h>

#define BLOCK 128

int main() {
  ssize_t sz;
  char buffer[BLOCK];

  while((sz = read(STDIN_FILENO, buffer, BLOCK)) > 0) {
    write(STDOUT_FILENO, buffer, (size_t) sz);
  }
}
