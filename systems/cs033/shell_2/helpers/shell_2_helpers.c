#include "helpers.h"

// deals with signals sent to processes
void handle_status(job_list_t *job_list, int status, pid_t pid, pid_t w_pid) {

  // if a background process
  if(!w_pid)
   return;

  if(WIFEXITED(status)) {
    char err_msg[BASE];
    int num_chars = sprintf(err_msg, "[%i] (%d) terminated with exit status %d\n", get_job_jid(job_list, pid), pid, WEXITSTATUS(status));
    if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
      exit(1);
  }

  if(WIFSTOPPED(status)) {
    char err_msg[BASE];
    int num_chars = sprintf(err_msg, "[%i] (%d) suspended by signal %d\n", get_job_jid(job_list, pid), pid, WSTOPSIG(status));
    if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
      exit(1);

    update_job_pid(job_list, pid, _STATE_STOPPED);
    kill(pid, SIGTSTP);
  }

  if(WIFSIGNALED(status)) {
    char err_msg[BASE];
    int num_chars = sprintf(err_msg, "[%i] (%d) terminated by signal %d\n", get_job_jid(job_list, pid), pid, WTERMSIG(status));
    if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
      exit(1);
  }

  if(WIFCONTINUED(status)) {
    char err_msg[BASE];
    int num_chars = sprintf(err_msg, "[%i] (%d) resumed\n", get_job_jid(job_list, pid), pid);
    if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
      exit(1);

    update_job_pid(job_list, pid, _STATE_RUNNING);
  }

  // remove pid from list if it exited or CTRL-C
  if(WIFEXITED(status) || WIFSIGNALED(status)) {
    if(remove_job_pid(job_list, pid) == -1) {
      char err_msg[BASE];
      int num_chars = sprintf(err_msg, "problem removing job from list\n");
      if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
        exit(1);
    }
  }

  return;
}

// if process exited normally, add to list and handle whatever status was returned
void update_and_handle(int status, char* cmd, job_list_t* job_list, int* count, pid_t pid, pid_t w_pid) {
  if(status != FAILURE) {
    process_state_t st = status == SIGTSTP ? _STATE_STOPPED : _STATE_RUNNING;

    // update list of processes
    if(add_job(job_list, *count, pid, st, cmd) == -1) {
      char err_msg[BASE];
      int num_chars = sprintf(err_msg, "process: problem adding to table\n");
      if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
        exit(1);
    }

    // if w_pid equals 0, process running in background
    if(!w_pid) {
      char err_msg[BASE];
      int num_chars = sprintf(err_msg, "[%i] (%d) \n", get_job_jid(job_list, pid), pid);
      if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
        exit(1);
    }

    // handle status from child process
    handle_status(job_list, status, pid, w_pid);
    *count = *count + 1;
  }
}

// use for the builtin commands fg and bg
int fg_bg(char* cmd, char* file, job_list_t* job_list) {
  // check that command starts with '%'
  if(file[0] != '%') {
    char err_msg[BASE];
    int num_chars = sprintf(err_msg, "%s: malformed job cmd\n", cmd);
    if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
      exit(1);

    return -1;
  }

  // # of file
  int num = atoi(file + 1);

  // get pid of job
  pid_t pid;
  if((pid = get_job_pid(job_list, num)) == -1) {
    char err_msg[BASE];
    int num_chars = sprintf(err_msg, "unable to find job\n");
    if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
      exit(1);

    return -1;
  }

  bool fg_process = (!strcmp(cmd, "fg"));

  // update job's status
  if(update_job_pid(job_list, pid, _STATE_RUNNING) == -1) {
    char err_msg[BASE];
    int num_chars = sprintf(err_msg, "unable to update job\n");
    if(write(STDERR_FILENO, err_msg, (size_t) num_chars) < 0)
      exit(1);

    return -1;
  }

  // if moving the process to the foreground, CONT pgid and change window to pgid
  if(fg_process) {
    signal(SIGTTIN, SIG_IGN);
    kill((pid * -1), SIGCONT);
    tcsetpgrp(STDIN_FILENO, getpgid(pid));
  }
  else {
    kill((pid * -1), SIGCONT);
  }

  return pid;
}

// any command will likely start with '/' or './', so check this before changing options for waitpid()
bool check_first_two(char* s) {
  if(s[0] != '.' && s[0] != '/')
    return false;
  else
    return true;
}
