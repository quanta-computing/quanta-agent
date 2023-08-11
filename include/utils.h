#ifndef MONIKOR_UTILS_H_H
#define MONIKOR_UTILS_H_H

#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <curl/curl.h>

#include "io_handler.h"
#include "monikor.h"

typedef struct _http_response_t {
  long code;
  char *data;
  size_t size;
  void (*callback)(struct _http_response_t *response, CURLcode result);
  void *userdata;
} http_response_t;


#define MNK_COMMAND_EXEC_STATE_PROCESS_DONE (1 << 0)
#define MNK_COMMAND_EXEC_STATE_READ_DONE (1 << 1)
#define MNK_COMMAND_EXEC_STATE_FAILED (1 << 2)
#define MNK_COMMAND_EXEC_STATE_FINISHED (MNK_COMMAND_EXEC_STATE_PROCESS_DONE + MNK_COMMAND_EXEC_STATE_READ_DONE)
#define monikor_command_finished(state) ((state & MNK_COMMAND_EXEC_STATE_FINISHED) == MNK_COMMAND_EXEC_STATE_FINISHED)
#define monikor_command_failed(state) (state & MNK_COMMAND_EXEC_STATE_FAILED)

typedef struct _command_exec {
  uint8_t state;

  pid_t pid;
  int status;

  monikor_io_handler_t *output_handler;
  size_t output_size;
  char *output;

  void *data;
  void (*callback)(struct _command_exec *output);
} command_exec_t;

char *monikor_read_file(const char *filepath);
char *monikor_read_fd(int fd);

int monikor_http_get(monikor_t *mon, const char *url, long timeout,
  void (*callback)(http_response_t *response, CURLcode result), void *data);
int monikor_http_get_with_headers(monikor_t *mon, const char *url, long timeout,
  void (*callback)(http_response_t *response, CURLcode result),
  void *data, struct curl_slist *headers);

monikor_io_handler_t *monikor_net_exchange(const char *host, const char *port, char *send,
void (*callback)(char *response, void *data), void *data);

monikor_process_handler_t *monikor_command_exec(const char *command, char *const argv[],
  void (*callback)(command_exec_t *output), void *data);
void monikor_command_free(monikor_process_handler_t *handler);
void monikor_command_register_io_handlers(monikor_t *mon, monikor_process_handler_t *handler);
void monikor_command_unregister_io_handlers(monikor_t *mon, monikor_process_handler_t *handler);

// DEBUG stuff
void dump_metric_list(monikor_metric_list_t *list);
void dump_store_size(monikor_metric_store_t *store);

#endif /* end of include guard: MONIKOR_UTILS_H_H */
