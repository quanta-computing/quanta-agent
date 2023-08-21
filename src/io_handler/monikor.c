#include <sys/wait.h>
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

static int monikor_process_watched(monikor_t *mon, pid_t pid) {
  monikor_process_handler_t *handler;

  for (handler = mon->io_handlers.processes.first
  ; handler && handler->pid != pid
  ; handler = handler->next);
  return handler != NULL;
}

void monikor_process_exited(monikor_t *mon, pid_t pid) {
  monikor_process_handler_t *handler;
  int status = 0;

  if (!monikor_process_watched(mon, pid))
    return;
  if (waitpid(pid, &status, 0) == -1) {
    monikor_log(LOG_WARNING, "monikor_process_exited: process %d, waitpid error: %s\n",
      pid, strerror(errno));
  }
  for (handler = mon->io_handlers.processes.first; handler; handler = handler->next) {
    if (handler->pid != pid)
      continue;
    if (handler->callback)
      handler->callback(handler, status);
  }
}
