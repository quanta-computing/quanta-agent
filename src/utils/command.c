#include <unistd.h>
#include <sys/wait.h>

#include "monikor.h"
#include "utils.h"

#define MNK_CMD_READ_SIZE 4096


static void maybe_finish_command_exec(command_exec_t *command) {
  if (monikor_command_finished(command->state)) {
    if (command->callback) {
      command->callback(command);
    }
  }
}

static void handle_command_output(monikor_io_handler_t *handler, __attribute__((unused)) uint8_t mode) {
  command_exec_t *cmd = (command_exec_t *)handler->data;
  int read_bytes;

  do {
    if (!(cmd->output = realloc(cmd->output, cmd->output_size + MNK_CMD_READ_SIZE + 1)))
      goto err;
    read_bytes = read(cmd->output_handler->fd, cmd->output + cmd->output_size, MNK_CMD_READ_SIZE);
    if (read_bytes == -1)
      goto err;
    cmd->output_size += read_bytes;
    cmd->output[cmd->output_size] = 0;
  } while (read_bytes == MNK_CMD_READ_SIZE);
  if (read_bytes == 0) {
    cmd->state |= MNK_COMMAND_EXEC_STATE_READ_DONE;
    close(cmd->output_handler->fd);
    cmd->output_handler->fd = -1;
  }
  maybe_finish_command_exec(cmd);
  return;

err:
  monikor_log(LOG_DEBUG, "command: failed in handle_command_output for process %d\n", cmd->pid);
  cmd->state |= MNK_COMMAND_EXEC_STATE_READ_DONE;
  cmd->state |= MNK_COMMAND_EXEC_STATE_FAILED;
  free(cmd->output);
  cmd->output_size = 0;
  cmd->output = NULL;
  close(cmd->output_handler->fd);
  cmd->output_handler->fd = -1;
}

static void handle_process_exit(monikor_process_handler_t *handler, int status) {
  command_exec_t *cmd = (command_exec_t *)handler->data;

  cmd->status = status;
  cmd->state |= MNK_COMMAND_EXEC_STATE_PROCESS_DONE;
  if (!WIFEXITED(status) || WEXITSTATUS(status)) {
    cmd->state |= MNK_COMMAND_EXEC_STATE_FAILED;
  }
  maybe_finish_command_exec(cmd);
}

static command_exec_t *start_command(const char *command, char *const argv[]) {
  command_exec_t *cmd = NULL;
  int pipefd[2];

  if (pipe(pipefd) == -1
  || !(cmd = malloc(sizeof(*cmd)))
  || !(cmd->output_handler = monikor_io_handler_new(pipefd[0], MONIKOR_IO_HANDLER_RD, &handle_command_output, (void *)cmd))) {
    goto err;
  }
  cmd->state = 0;
  cmd->data = NULL;
  cmd->output = NULL;
  cmd->output_size = 0;
  cmd->callback = NULL;
  cmd->status = 0;
  cmd->pid = fork();
  if (cmd->pid == -1) {
    goto err;
  }
  if (cmd->pid == 0) {
    close(pipefd[0]);
    if (dup2(pipefd[1], STDOUT_FILENO) == -1
    || execvp(command, argv) == -1)
      exit(1);
  } else {
    close(pipefd[1]);
  }

  return cmd;

err:
  if (cmd)
    free(cmd->output_handler);
  free(cmd);
  return NULL;
}

monikor_process_handler_t *monikor_command_exec(const char *command, char *const argv[],
void (*callback)(command_exec_t *output), void *data) {
  command_exec_t *cmd = NULL;
  monikor_process_handler_t *handler = NULL;


  if (!(cmd = start_command(command, argv))
  || !(handler = monikor_process_handler_new(cmd->pid, &handle_process_exit, cmd))) {
    goto err;
  }
  cmd->callback = callback;
  cmd->data = data;

  return handler;

err:
  free(cmd);
  free(handler);
  return NULL;
}

void monikor_command_free(monikor_process_handler_t *handler) {
  command_exec_t *cmd = (command_exec_t *)handler->data;

  if (cmd) {
    free(cmd->output_handler);
    free(cmd->output);
  }
  free(cmd);
  free(handler);
}

void monikor_command_register_io_handlers(monikor_t *mon, monikor_process_handler_t *handler) {
  command_exec_t *cmd = (command_exec_t *)handler->data;

  monikor_register_processs(mon, handler);
  monikor_register_io_handler(mon, cmd->output_handler);
}

void monikor_command_unregister_io_handlers(monikor_t *mon, monikor_process_handler_t *handler) {
  command_exec_t *cmd = (command_exec_t *)handler->data;

  monikor_unregister_io_handler(mon, cmd->output_handler);
  monikor_unregister_processs(mon, handler);
}
