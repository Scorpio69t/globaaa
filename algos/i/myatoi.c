#include <stdio.h>
#include <stdbool.h>

int mystrlen(char* str);

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("%i: not enough input arguments (2)\n", argc);
    return 1;
  }

  /* get info */
  char* number = argv[1];
  int numlen = mystrlen(number);
  int result = 0;
  bool neg = false;

  for(int i = 0; i < numlen; i++) {
    if (number[i] < 48 || number[i] > 57) {
      if(!i && (number[i] == '-')) {
	neg = true;
        continue;
      } else {
        printf("%c: not a numeric character\n", number[i]);
        return 1;
      }
    }

    result = (result * 10) + (number[i] - 48);
  }

  if(neg)
    result = result * -1;

  printf("%s -> %d\n", number, result);
}

int mystrlen(char* str) {
  int i = 0;
  for(i = 0; str[i] != '\0'; i++) {  }

  return i;
}
