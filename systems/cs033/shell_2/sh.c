#include "helpers/helpers.h"
#include "jobs.h"

int main() {

  // ignore signals
  signal(SIGINT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);

  // initial buffer of size 512; will hold cmd-line input
  char* buff = malloc(BLOCK);
  zero_block(buff, BLOCK);

  // if call to malloc() fails, report error and exit with code 1
  if(!buff)
    error_exit("problem allocating memory\n");

  // use to keep track of chars read in
  ssize_t sz = 0;
  // keep track of number of jobs
  int count = 1;

  // initialize a list of jobs
  job_list_t* job_list = init_job_list();

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
    if(!arg_vec)
      error_exit("problem allocating memory\n");

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

    // if only whitespace + "\n", start over while waiting on any other processes
    if(!strcmp(cmd, "\n")) {
      pid_t pid;
      while((pid = get_next_pid(job_list)) != -1) {

	int options = WUNTRACED | WNOHANG | WCONTINUED;
	int status = 0;

        // wait parent process until child returns; wait will return 0 if running background
        pid_t w_pid = waitpid(pid, &status, options);

        // handle status from child process
        handle_status(job_list, status, pid, w_pid);
      }

      #ifdef PROMPT
      if (write(STDOUT_FILENO, "33sh> ", PROMPT_SIZE) < 0)
        exit(1);
      #endif

      // prepare buffer for new input
      zero_block(buff, (size_t) sz);
      free(arg_vec);
      continue;
    }

    // null terminate '\n' terminated string
    char* last = arg_vec[argc - 1];
    last[strlen(last) - 1] = '\0';

    // EXIT shell command
    if(!strncmp(cmd, "exit", EXIT_JOBS_SIZE)) {
      // check that they only typed "exit"
      if(argc == 1) {
        zero_block(buff, (size_t) sz);
        free(arg_vec);
        break;
      }
      else
	error_no_exit("exit: syntax error\n");
    }

    // CD shell command
    else if(!strncmp(cmd, "cd", CD_LN_RM_SIZE)) {
      // check that they gave a single dest. to cd
      if(argc == 1)
        error_no_exit("cd: syntax error\n");
     else
       change_dir(arg_vec);
    }

    // LN shell command
    else if(!strncmp(cmd, "ln", CD_LN_RM_SIZE)) {
      // check that they gave a dest and src to ln
      if(arg_num(arg_vec) != 3)
	error_no_exit("ln: syntax error\n");
     else
       create_ln(arg_vec);
    }

    // RM shell command
    else if(!strncmp(cmd, "rm", CD_LN_RM_SIZE)) {
      // check that they gave a file to rm
      if(argc != 2)
	error_no_exit("rm: syntax error\n");
     else
       rm_file(arg_vec);
    }

    // JOBS shell command
    else if(!strncmp(cmd, "jobs", EXIT_JOBS_SIZE)) {
     // count++;
      jobs(job_list);
    }

    // FGBG shell command
    else if(!strncmp(cmd, "fg", FGBG_SIZE) || !strncmp(cmd, "bg", FGBG_SIZE)) {

      if(argc != 2)
	error_no_exit("fg: syntax error\n");
      else {
	pid_t pid = fg_bg(cmd, arg_vec[1], job_list);
	if(pid == -1)
	  continue;

	bool fg = !strncmp(cmd, "fg", FGBG_SIZE);
	int options = fg ? WUNTRACED : WUNTRACED | WCONTINUED;


	int status = 0;
	pid_t w_pid = waitpid(pid, &status, options);

	if(!status) {
	  if(remove_job_pid(job_list, pid) == -1) {
	    error_no_exit("cound not remove from job list\n");
	    continue;
          }
	}
	else
          // handle status from child process
          handle_status(job_list, status, pid, w_pid);
      }
    }

    // deal with redirection; won't include cd, rm, ln, or exit
    else if((pos = find_redirection(arg_vec, argc)) >= 0) {

      if(argc < 3) {
	error_no_exit("not enough args to redirection\n");

        #ifdef PROMPT
        if (write(STDOUT_FILENO, "33sh> ", PROMPT_SIZE) < 0)
          exit(1);
        #endif

        zero_block(buff, (size_t) sz);
        free(arg_vec);
	continue;
      }

      // start setting up to fork
      pid_t pid;
      int status = 0;
      int new_fd;

      // set option to return even if the process is suspended (CTRL-Z)
      int options = WUNTRACED | WCONTINUED;

      // check to see if you should run the process in the background
      bool background = !strcmp(arg_vec[argc - 1], "&");

      // if you're supposed to run in background, don't wait for process to end
      if(background && check_first_two(cmd)) {
        options = options | WNOHANG;
        arg_vec[argc - 1] = NULL;
        argc -= 1;
      }

      if(!(pid = fork())) {
	// while there are still redirection symbols in arg_vec
        while(pos >= 0) {

	  // change group of the process to the same as its pid
	  setpgid(0, 0);

     	  // if '&' at the end of command, don't change tcsetpgrp + don't wait for process end
          if(!background)
	    tcsetpgrp(STDIN_FILENO, getpid());

          // too many redirection symbols
          if(count_redirection(arg_vec, argc, arg_vec[pos]) != 1)
	    error_exit("only one redirection allowed\n");

	  // check to see what kind of redirection this is
	  if(!strncmp("<", arg_vec[pos], 1)) {

	    // open file
	    if((new_fd = open(arg_vec[pos+1], O_RDONLY)) == -1)
	       error_exit("<: could not open file\n");

	    // duplicate fd; replace fd 0 with new_fd
	    if(dup2(new_fd, STDIN_FILENO) == -1)
	       error_exit("<: could not redirect\n");

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
	    if((new_fd = open(arg_vec[pos+1], O_CREAT | O_APPEND | O_WRONLY, RW_PERM)) == -1)
	      error_exit(">>: error opening file\n");

	    if(dup2(new_fd, STDOUT_FILENO) == -1)
	      error_exit(">>: could not redirect\n");

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
	    if((new_fd = open(arg_vec[pos+1], O_CREAT | O_TRUNC | O_WRONLY, RW_PERM)) == -1)
	      error_exit(">: error opening file\n");

	    if(dup2(new_fd, STDOUT_FILENO) == -1)
	      error_exit(">: could not redirect\n");

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

        // restore signals before executing child process
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);

	arg_vec[0] = local;
	execv(cmd, arg_vec);

	error_exit("execv: no such file or directory\n");
      }

      // wait parent process until child returns; wait will return 0 if running background
      pid_t w_pid = waitpid(pid, &status, options);

      if(!status && w_pid)
        count++;

      // if status != 0 then the child received some signal -
      // unless status and w_pid are both 0, then process in background
      if(status || (!status && !w_pid))
        update_and_handle(status, cmd, job_list, &count, pid, w_pid);

      // run waitpid on all processes in list
      pid_t old = pid;
      while((pid = get_next_pid(job_list)) != -1) {
        if(old == pid)
          continue;

        options = options | WNOHANG;

        // wait parent process until child returns; wait will return 0 if running background
        w_pid = waitpid(pid, &status, options);

        // handle status from child process
        handle_status(job_list, status, pid, w_pid);
      }
    }
    // all other bash commands
    else {
      pid_t pid;
      int status = 0;

      // set option to return even if the process is suspended (CTRL-Z)
      int options = WUNTRACED | WCONTINUED;

      // check to see if you should run the process in the background
      bool background = !strcmp(arg_vec[argc - 1], "&");

      // if you're supposed to run in background, don't wait for process to end
      if(background && check_first_two(cmd)) {
	options = options | WNOHANG;
        arg_vec[argc - 1] = NULL;
	argc -= 1;
      }

      if(!(pid = fork())) {
        // change group of the process to the same as its pid
	setpgid(0, 0);

	// if '&' at the end of command, don't change tcsetpgrp
        if(!background)
	  tcsetpgrp(STDIN_FILENO, getpid());

	int len = (int) strlen(cmd);
	char* local = first_slash(cmd + len, len);
        local += 1;

        // restore signals before executing child process
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);

	arg_vec[0] = local;
	execv(cmd, arg_vec);

	// if execv() fails
	error_exit("execv: no such file or directory\n");
      }

      // wait parent process until child returns; wait will return 0 if running background
      pid_t w_pid = waitpid(pid, &status, options);

      if(!status && w_pid)
        count++;

      // if status != 0 then the child received some signal -
      // unless status and w_pid are both 0, then process in background
      if(status || (!status && !w_pid))
        update_and_handle(status, cmd, job_list, &count, pid, w_pid);

      // run waitpid on all processes in list
      pid_t old = pid;
      while((pid = get_next_pid(job_list)) != -1) {
        if(old == pid)
	  continue;

	options = options | WNOHANG;

        // wait parent process until child returns; wait will return 0 if running background
        w_pid = waitpid(pid, &status, options);

        // handle status from child process
        handle_status(job_list, status, pid, w_pid);
      }
   }

    // change control back to parent process
    tcsetpgrp(STDIN_FILENO, getpgid(getpid()));

    // prepare buffer for new input
    zero_block(buff, (size_t) sz);
    free(arg_vec);

    #ifdef PROMPT
    if (write(STDOUT_FILENO, "33sh> ", PROMPT_SIZE) < 0)
      exit(1);
    #endif
  }

  cleanup_job_list(job_list);
  free(buff);
  return 0;
}
