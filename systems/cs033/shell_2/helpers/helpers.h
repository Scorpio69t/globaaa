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
#include "../jobs.h"

// common size of buffers
#define BASE 128

// initial malloc
#define BLOCK 512

// used in strncmp
#define EXIT_JOBS_SIZE 4

// used in strncmp
#define CD_LN_RM_SIZE 2

// "33sh> "
#define PROMPT_SIZE 6

// used when allocating arg_vec
#define ARG_SIZE 32

// open file with read-write permissions
#define RW_PERM 0600

// size of builtin fg | bg
#define FGBG_SIZE 2

// failure of a process
#define FAILURE 256

// prototypes shell 1
void error_exit(char* msg);
void error_no_exit(char* msg);
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

// prototypes shell 2
void handle_status(job_list_t *job_list, int status, pid_t pid, pid_t w_pid);
bool check_first_two(char* s);
void update_and_handle(int status, char* cmd, job_list_t* job_list, int* count, pid_t pid, pid_t w_pid);
int fg_bg(char* cmd, char* file, job_list_t* job_list);
