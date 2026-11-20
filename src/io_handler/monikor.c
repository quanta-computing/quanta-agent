#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#include "monikor.h"
#include "io_handler.h"

void monikor_register_io_handler(monikor_t *mon, monikor_io_handler_t *handler) {
  monikor_io_handler_list_push(&mon->io_handlers, handler);
}

void monikor_unregister_io_handler(monikor_t *mon, monikor_io_handler_t *handler) {
  monikor_io_handler_list_remove(&mon->io_handlers, handler);
}

void monikor_register_process(monikor_t *mon, monikor_process_handler_t *handler) {
  monikor_io_handler_list_push_process(&mon->io_handlers, handler);
}

void monikor_unregister_process(monikor_t *mon, monikor_process_handler_t *handler) {
  monikor_io_handler_list_remove_process(&mon->io_handlers, handler);
}

void monikor_reap_exited_children(monikor_t *mon) {
  pid_t pid;
  int status;

  // SIGCHLD does not queue: one signal may stand for several exited children
  while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    monikor_process_exited(mon, pid, status);
  if (pid == -1 && errno != ECHILD)
    monikor_log(LOG_WARNING, "monikor_reap_exited_children: waitpid error: %s\n", strerror(errno));
}

void monikor_process_exited(monikor_t *mon, pid_t pid, int status) {
  monikor_process_handler_t *handler;
  monikor_process_handler_t *next;

  // callbacks may unregister and free their own handler (reap_process_callback)
  for (handler = mon->io_handlers.processes.first; handler; handler = next) {
    next = handler->next;
    if (handler->pid != pid)
      continue;
    if (handler->callback)
      handler->callback(handler, status);
  }
}

static void reap_process_callback(monikor_process_handler_t *handler, int status) {
  monikor_t *mon = (monikor_t *)handler->data;
  int exitval = -1;
  char *status_str = NULL;

  if (WIFEXITED(status)) {
    status_str = "exited with code";
    exitval = WEXITSTATUS(status);
  } else if (WIFSIGNALED(status)) {
    status_str = "killed with signal";
    exitval = WTERMSIG(status);
  }
  if (status_str)
    monikor_log(LOG_INFO, "reaped process %d: %s %d\n", handler->pid, status_str, exitval);
  else
    monikor_log(LOG_INFO, "reaped process %d\n", handler->pid);
  monikor_unregister_process(mon, handler);
  free(handler);
}

int monikor_reap_process(monikor_t *mon, pid_t pid) {
  monikor_process_handler_t *reaper;

  if (!(reaper = monikor_process_handler_new(pid, &reap_process_callback, (void *)mon)))
    return -1;
  monikor_register_process(mon, reaper);
  if (kill(pid, SIGKILL) == -1 && errno != ESRCH) {
    monikor_log(LOG_WARNING, "could not kill process %d: %s\n", pid, strerror(errno));
    return -1;
  }
  return 0;
}
