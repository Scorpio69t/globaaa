#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define BUFFER 256
#define NEGATE(x) (-1) * x

int main() {

        // copy pathname into buffer so you can use strtok()
        char buffer[BUFFER];
        strcpy(buffer, "//////");

        // special case "/"
        char* dir = strtok(buffer, "/");
        if(!dir) {
	  printf("%s: special case: %i\n", "/", NEGATE(1));
          return 0;
        }

        while(true) {
          char* dname = strtok(NULL, "/");
          if(!dname) {
	    printf("%s: special case: %i\n", dir, NEGATE(2));
            return 0;
          }
          else
            dir = dname;
	}
}
