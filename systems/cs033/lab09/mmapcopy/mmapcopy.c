#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: mmap <source filepath> <destination file path>\n");
        exit(1);
    }

    int src_fd, dst_fd;
    void *src, *dst;
    size_t filesize;
    off_t offset = 0;
    struct stat st;

    // TODO: Fill in the function outline below.

    // (1) Open the files passed to the program and assign
    //     file descriptors to them.
    if((src_fd = open(argv[1], O_RDONLY)) == -1) {
      printf("Could not open source file\n");
      exit(1);
    }

    if((dst_fd = open(argv[2], O_RDWR | O_CREAT, 0600)) == -1) {
      printf("Could not open destination file\n");
      exit(1);
    }

    // (2) Find the size of source file.
    fstat(src_fd, &st);
    filesize = st.st_size;

    // (3) Make sure that the destination file is the same size as source file.
    ftruncate(dst_fd, filesize);

    // (4) Map virtual addresses to source and destination file descriptors.
    src = mmap(0, filesize, PROT_READ, MAP_SHARED, src_fd, offset);
    if(src == (void*) -1) {
      printf("%s\n", strerror(errno));
      exit(1);
    }

    dst = mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dst_fd, offset);
    if(dst == (void*) -1) {
      printf("%s\n", strerror(errno));
      exit(1);
    }

    // (5) Copy pages from source to destination addresses.
    memcpy(dst, src, filesize);

    // (6) Unmap virtual addresses and close file descriptors.
    if(munmap(src, filesize) || munmap(dst, filesize)) {
      printf("Problem unmapping virtual addresses\n");
      exit(1);
    }

    close(src_fd);
    close(dst_fd);

    // To test, run ./mmapcopy cit.jpg <output_file_path> and then
    // cs033_file_equals cit.jpg <output_file_path>
    return 0;
}
