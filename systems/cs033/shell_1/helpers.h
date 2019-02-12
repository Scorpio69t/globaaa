#include <sys/uio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

// common size of buffers
#define BASE 128

// initial malloc
#define BLOCK 512

// used in strncmp
#define EXIT_SIZE 4

// used in strncmp
#define CD_LN_RM_SIZE 2

// "33sh> "
#define PROMPT_SIZE 6

// used when allocating arg_vec
#define ARG_SIZE 32

// open file with read-write permissions
#define RW_PERM 0600

// prototypes
void zero_block(char* ptr, size_t sz);
char* find_cmd(char* buffer, ssize_t size);
size_t arg_num(char** array);
char* first_slash(char* ptr, int sz);
void change_dir(char** argv);
void create_ln(char** argv);
void rm_file(char** argv);
int find_redirection(char** argv, size_t sz);
size_t count_redirection(char** argv, size_t sz, char* r);
void create_args(char** argv, size_t pos, size_t size);
