#include "helpers.h"

// to make my call to malloc() function the same as calloc(); O(sz)
void zero_block(char* ptr, size_t sz) {
  for(size_t i = 0; i < sz; i++, ptr++)
    *ptr = 0;
}

// find cmd in buffer; O(sz)
char* find_cmd(char* buffer, ssize_t size) {
  char* word = NULL;

  for(ssize_t i = 0; i < size; i++, buffer++) {
    char c = buffer[0];
    if(!c)
      break;
    // skip over any whitespace at the beginning of cmd-line
    else if((c == '\t' || c == ' ' || c == '\n') && !word)
      continue;
    // place null terminator at end of string
    else if((c == '\t' || c == ' ' || c == '\n') && word) {
      buffer[0] = '\0';
      break;
    }
    // find end of string
    else if(word)
      continue;
    else
      word = buffer;
 }

  return word;
}

// return the number of words in array
size_t arg_num(char** array) {
  size_t count = 0;
  while(array[count]) {
    count++;
  }

  return count;
}

// find first slash in a path, reading backwards; O(sz)
char* first_slash(char* ptr, int sz) {
  char* slash = NULL;

  for(; sz >= 0; sz -= 1) {
    ptr = ptr - 1;
    if(((char) *ptr) == '/') {
      slash = ptr;
      break;
    }
  }

  return slash;
}

// find the position of the redirection symbol in the arg_vec; -1 if none
int find_redirection(char** argv, size_t sz) {
  for(size_t i = 0; i < sz; i++) {
    // find redirection pos
    if(!strncmp(argv[i], "<", 1) || !strncmp(argv[i], ">", 1) || !strncmp(argv[i], ">>", 1)) {
      return (int) i;
    }
  }

  return -1;
}

// count # of redirection symbols in the arg_vec
size_t count_redirection(char** argv, size_t sz, char* r) {
  size_t count = 0;
  for(size_t i = 0; i < sz; i++) {
    // find redirection pos
    if(!strncmp(argv[i], r, strlen(r)))
      count++;
  }

  return count;
}

// remove redirection operator
void create_args(char** argv, size_t pos, size_t size) {
  for(size_t i = pos; i < size; i++) {
    argv[i] = argv[i + 1];
  }
}

// change directory
void change_dir(char** argv) {
  // get the current directory path
  char* current_dir = calloc(1, BASE);
  (void) getcwd(current_dir, BASE);

  // if getcwd fails
  if(!current_dir) {
    char err_msg[BASE];
    int num_chars = sprintf(err_msg, "cd: could not get working directory\n");
    if (write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
      exit(1);
  }

  // get destination
  char* dest = argv[1];
  char* full_path = NULL;

  if(!strcmp(dest, ".")) {
    // stay in current location
    full_path = current_dir;
  }
  else if(!strcmp(dest, "..")) {
    // find first slash from end of path
    int len = (int) strlen(current_dir);
    char* ptr = current_dir + len;
    char* slash = first_slash(ptr, len);

    // create a new string without present destination
    slash[0] = '\0';

    // return new path
    full_path = current_dir;
  }
  else {
    char slash[] = "/";
    char* added_slash = strcat(slash, dest);
    // concat directory path with directory you want to cd into
    full_path = strcat(current_dir, added_slash);
  }

  free(current_dir);

  if(chdir(full_path)) {
    char err_msg[BASE];
    if (errno == ENOENT) {
      int num_chars = sprintf(err_msg, "cd: could not find directory: %s\n", argv[1]);
      if (write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
        exit(1);
    }
    else {
      int num_chars = sprintf(err_msg, "cd: problem changing directory\n");
      if (write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
        exit(1);
    }
  }
}

// create link
void create_ln(char** argv) {
  // create strings to files
  char* exist = argv[1];
  char* lnk = argv[2];

  // if link produces an error
  if(link(exist, lnk)) {
    char err_msg[BASE];
    if(errno == EEXIST) {
      int num_chars = sprintf(err_msg, "ln: file exists\n");
      if (write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
        exit(1);
    }
    else if(errno == ENOENT) {
      int num_chars = sprintf(err_msg, "ln: no such file or directory\n");
      if (write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
        exit(1);
    }
    else {
      int num_chars = sprintf(err_msg, "ln: error creating link\n");
      if (write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
        exit(1);
    }
  }
}

// remove file
void rm_file(char** argv) {
  // if unlink produces an error
  if(unlink(argv[1])) {
    char err_msg[BASE];
    if(errno == EISDIR) {
      int num_chars = sprintf(err_msg, "rm: is a directory\n");
      if (write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
        exit(1);
    }
    else if(errno == ENOENT) {
      int num_chars = sprintf(err_msg, "rm: no such file or directory\n");
      if (write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
        exit(1);
    }
    else {
      int num_chars = sprintf(err_msg, "rm: error removing file\n");
      if (write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
        exit(1);
    }
  }
}
