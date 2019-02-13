#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

#include "monikor.h"
#include "postfix.h"

static void exec_mailq(const char *path, int fd_out) {
  if (dup2(fd_out, 1) == -1
  || execl(path, path, NULL) == -1)
    exit(1);
}

static char *mailq_read(const char *path) {
  pid_t pid;
  int pipefd[2];
  char *content = NULL;
  int status;

  if (pipe(pipefd) == -1
  || (pid = fork()) == -1) {
      return NULL;
  }
  if (pid) {
    close(pipefd[1]);
    content = monikor_read_fd(pipefd[0]);
    waitpid(pid, &status, 0);
    if (!WIFEXITED(status) || WEXITSTATUS(status)) {
      free(content);
      return NULL;
    }
  } else {
    close(pipefd[0]);
    exec_mailq(path, pipefd[1]);
  }
  return content;
}

static int mailq_parse(char *content) {
  int n;
}

int postfix_fetch_metrics(monikor_t *mon, struct timeval *clock) {
  char *content;
  int n;

  if (!(content = mailq_read("/usr/bin/mailq")))
    return 0;
  n = mailq_parse(content);
  free(content);
  return n;
}
