#include <stdio.h>
#include <unistd.h>

int main() {

  while(1) {
    char buffer[10];
    read(0, buffer, 10);
  }
}
