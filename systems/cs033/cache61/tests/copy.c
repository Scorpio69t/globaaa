#include <stdio.h>

int main() {

  FILE* in = fopen("hamlet", "r");

  if(!in)
    return 1;

  FILE* out = fopen("out", "w");

  if(!out)
    return 2;

  unsigned char* buffer[9];
  int nread = 0;

  while((nread = fread(buffer, 8, 1, in))) {
    buffer[nread] = '\0';
    fwrite(buffer, 8, 1, out);
  }

  fclose(in);
  fclose(out);
}
