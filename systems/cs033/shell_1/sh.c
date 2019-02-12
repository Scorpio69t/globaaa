#include "helpers.h"

int main() {

  // initial buffer of size 512; will hold cmd-line input
  char* buff = malloc(BLOCK);
  zero_block(buff, BLOCK);

  // if call to malloc() fails, report error and exit with code 1
  if(!buff) {
    char err_msg[BASE];
    int num_chars = sprintf(err_msg, "problem allocating memory\n");
    write(STDERR_FILENO, err_msg, (size_t) num_chars);
    exit(1);
  }

  // use to keep track of chars read in
  ssize_t sz = 0;

  #ifdef PROMPT
  if (write(STDOUT_FILENO, "33sh> ", PROMPT_SIZE) < 0)
    exit(1);
  #endif

  // continue while input != CTRL-D
  while((sz = read(STDIN_FILENO, buff, BLOCK)) > 0) {

    // length of word found in buffer
    ssize_t arg_len = 0;
    // offset of new word from beginning of buff
    ssize_t offset = 0;
    // position of possible redirection
    int pos;

    // create an array to hold all arguments; up to 32
    char** arg_vec = malloc(sizeof(char*) * ARG_SIZE);
    zero_block((char*) arg_vec, sizeof(char*) * ARG_SIZE);

    // similar error msg to allocating buff
    if(!arg_vec) {
      char err_msg[BASE];
      int num_chars = sprintf(err_msg, "problem allocating memory\n");
      write(STDERR_FILENO, err_msg, (size_t) num_chars);
      exit(1);
    }

    // use to parse buff
    int k = 0;
    for(ssize_t update_len = sz; update_len > 0; k++) {
      // find word in buff
      arg_vec[k] = find_cmd(buff + offset, update_len);
      char* found_word = arg_vec[k];

      // if no input on cmd-line
      if(!found_word)
        break;

      // find length of word + '\0'; size_t subset of ssize_t
      arg_len = ((ssize_t) strlen(found_word)) + 1;

      // this should point in the buffer right after the found word
      offset = found_word - buff + arg_len;

      // new length of buffer
      update_len = sz - offset;
    }

    // primary command & # of arguments
    char* cmd = arg_vec[0];
    size_t argc = arg_num(arg_vec);

    // if no input, skip and wait for more input
    if(!cmd)
      continue;

    // EXIT shell command
    if(!strncmp(cmd, "exit", EXIT_SIZE)) {
      // check that they only typed "exit"
      if(argc == 1)
        break;
      else {
        char err_msg[BASE];
        int num_chars = sprintf(err_msg, "exit: syntax error\n");

	// if write error, return exit code of 1
	if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
	  exit(1);
      }
    }

    // CD shell command
    else if(!strncmp(cmd, "cd", CD_LN_RM_SIZE)) {
      // check that they gave a single dest. to cd
      if(argc == 1) {
        char err_msg[BASE];
        int num_chars = sprintf(err_msg, "cd: syntax error\n");
	if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
	  exit(1);
      }
     else
       change_dir(arg_vec);
    }

    // LN shell command
    else if(!strncmp(cmd, "ln", CD_LN_RM_SIZE)) {
      // check that they gave a dest and src to ln
      if(arg_num(arg_vec) != 3) {
        char err_msg[BASE];
        int num_chars = sprintf(err_msg, "ln: syntax error\n");
	if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
	  exit(1);
      }
     else
       create_ln(arg_vec);
    }

    // RM shell command
    else if(!strncmp(cmd, "rm", CD_LN_RM_SIZE)) {
      // check that they gave a file to rm
      if(argc != 2) {
        char err_msg[BASE];
        int num_chars = sprintf(err_msg, "rm: syntax error\n");
	if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
	  exit(1);
      }
     else
       rm_file(arg_vec);
    }

    // deal with redirection; won't include cd, rm, ln, or exit
    else if((pos = find_redirection(arg_vec, argc)) >= 0) {

      // start setting up to fork
      pid_t pid;
      int status;
      int new_fd;

      if(!(pid = fork())) {
	// while there are still redirection symbols in arg_vec
        while(pos >= 0) {

          // too many redirection symbols
          if(count_redirection(arg_vec, argc, arg_vec[pos]) != 1) {
            char err_msg[BASE];
            int num_chars = sprintf(err_msg, "only one redirection allowed\n");
            if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
              exit(1);

	    // exit from process
	    exit(1);
          }

	  // check to see what kind of redirection this is
	  if(!strncmp("<", arg_vec[pos], 1)) {

	    // open file
	    if((new_fd = open(arg_vec[pos+1], O_RDONLY)) == -1) {
	      char err_msg[BASE];
	      int num_chars = sprintf(err_msg, "<: could not open file\n");
              if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
                exit(1);

	      // exit from process
	      exit(1);
            }

	    // duplicate fd; replace fd 0 with new_fd
	    if(dup2(new_fd, STDIN_FILENO) == -1) {
	      char err_msg[BASE];
	      int num_chars = sprintf(err_msg, "<: could not redirect\n");
              if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
                exit(1);

	      // exit from process
	      exit(1);
	    }

	    close(new_fd);

	    // remove filename; safe to cast since we know pos >= 0
            create_args(arg_vec, (size_t) pos + 1, argc);
	    // remove redirection
            create_args(arg_vec, (size_t) pos, argc - 1);

	    // update vars
	    argc -= 2;
            cmd = arg_vec[0];
          }

	  else if (pos >= 0 && !strncmp(">>", arg_vec[pos], 2)) {

	    // open file
	    if((new_fd = open(arg_vec[pos+1], O_CREAT | O_APPEND | O_WRONLY, RW_PERM)) == -1) {
	      char err_msg[BASE];
	      int num_chars = sprintf(err_msg, ">>: error opening file\n");
              if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
                exit(1);

	      // exit from process
	      exit(1);
            }

	    if(dup2(new_fd, STDOUT_FILENO) == -1) {
	      char err_msg[BASE];
	      int num_chars = sprintf(err_msg, ">>: could not redirect\n");
              if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
                exit(1);

	      // exit from process
	      exit(1);
	    }

	    close(new_fd);

	    // prepare argv for execv(); remove filename
            create_args(arg_vec, (size_t) pos + 1, argc);
	    // remove redirection
            create_args(arg_vec, (size_t) pos, argc - 1);

	    // update vars
	    cmd = arg_vec[0];
	    argc -= 2;
	  }

	  else if(pos >= 0 && !strncmp(">", arg_vec[pos], 1)) {

	    // open file
	    if((new_fd = open(arg_vec[pos+1], O_CREAT | O_TRUNC | O_WRONLY, RW_PERM)) == -1) {
	      char err_msg[BASE];
	      int num_chars = sprintf(err_msg, ">: error opening file\n");
              if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
                exit(1);

	      // exit from process
	      exit(1);
            }

	    if(dup2(new_fd, STDOUT_FILENO) == -1) {
	      char err_msg[BASE];
	      int num_chars = sprintf(err_msg, ">: could not redirect\n");
              if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
                exit(1);

	      // exit from process
	      exit(1);
	    }

	    close(new_fd);

	    // prepare argv for execv(); remove filename
            create_args(arg_vec, (size_t) pos + 1, argc);
	    // remove redirection
            create_args(arg_vec, (size_t) pos, argc - 1);

	    // update vars
	    cmd = arg_vec[0];
	    argc -= 2;
	  }

	  pos = find_redirection(arg_vec, argc);
        }

        // find name of command from full path
	int len = (int) strlen(cmd);
	char* local = first_slash(cmd + len, len);
        local += 1;

	arg_vec[0] = local;
	execv(cmd, arg_vec);

	char err_msg[BASE];
	int num_chars = sprintf(err_msg, "execv: no such file or directory\n");
        if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
          exit(1);

	// will only get here if error in execv()
	exit(1);
      }

      // wait parent procesc until child returns 1
      waitpid(pid, &status, 0);
    }

    // all other bash commands
    else {
      pid_t pid;
      int status;

      if(!(pid = fork())) {
	int len = (int) strlen(cmd);
	char* local = first_slash(cmd + len, len);
        local += 1;

	arg_vec[0] = local;
	execv(cmd, arg_vec);

	// if execv() fails
	char err_msg[BASE];
	int num_chars = sprintf(err_msg, "execv: no such file or directory\n");
        if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
          exit(1);

	exit(1);
      }

      // wait parent procesc until child returns 1
      waitpid(pid, &status, 0);
    }

    // prepare buffer for new input
    zero_block(buff, (size_t) sz);
    free(arg_vec);

    #ifdef PROMPT
    if (write(STDOUT_FILENO, "33sh> ", PROMPT_SIZE) < 0)
      exit(1);
    #endif
  }

  free(buff);
  return 0;
}
