#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define SIZE 10

/* question: i want to see what's printed out of a buffer partially filled with NULL
 *
 * answer: so, it prints out some nonsense characters (i'm assuming the ascii equivalent of 0).
 * this isn't so dumb to wonder. for example, let's say you call read(SIZE) on a file containing only zeros. read()
 * returns the number of characters successfully read. read() on a file of zeros, though, will update the
 * cursor within the file but return 0. conceivably, then, writing nothing could also update the file
 * cursor without actually outputting anything to the file.
 */

unsigned char buffer[SIZE];

int main() {
    buffer[2] = 'h';
    buffer[3] = 'i';

    /* let's open a file to write to */
    int out = open("out", O_WRONLY | O_CREAT | O_TRUNC);
    write(out, buffer, SIZE);
    close(out);
}
