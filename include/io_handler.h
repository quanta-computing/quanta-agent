#ifndef MONIKOR_IO_HANDLER_H_
#define MONIKOR_IO_HANDLER_H_

#include <stdint.h>
#include <unistd.h>
#include <curl/curl.h>

#define MONIKOR_IO_HANDLER_RD (1 << 0)
#define MONIKOR_IO_HANDLER_WR (1 << 1)
#define MONIKOR_IO_HANDLER_RDWR (IO_HANDLER_RD | IO_HANDLER_WR)

typedef struct _monikor_io_handler {
  int fd;
  uint8_t mode;
  void (*callback)(struct _monikor_io_handler *handler, uint8_t mode);
  void *data;

  // Internal use only
  struct _monikor_io_handler *prev;
  struct _monikor_io_handler *next;
} monikor_io_handler_t;

typedef struct _monikor_curl_handler {
  CURL *curl;
  void *data;
  void (*callback)(struct _monikor_curl_handler *handler, CURLcode result);

  // Internal use
  struct _monikor_curl_handler *prev;
  struct _monikor_curl_handler *next;
} monikor_curl_handler_t;

typedef struct _monikor_process_handler {
  pid_t pid;

  void (*callback)(struct _monikor_process_handler *handler, int status);
  void *data;

  // Internal use only
  struct _monikor_process_handler *prev;
  struct _monikor_process_handler *next;
} monikor_process_handler_t;


typedef struct {
  CURLM *curl;
  monikor_curl_handler_t *first;
  monikor_curl_handler_t *last;
} monikor_curl_handler_list_t;

typedef struct {
  monikor_process_handler_t *first;
  monikor_process_handler_t *last;
} monikor_process_handler_list_t;

typedef struct {
  monikor_io_handler_t *first;
  monikor_io_handler_t *last;
  monikor_curl_handler_list_t curl;
  monikor_process_handler_list_t processes;
} monikor_io_handler_list_t;


monikor_io_handler_t *monikor_io_handler_new(int fd, uint8_t mode,
  void (*callback)(monikor_io_handler_t *handler, uint8_t mode), void *data);

monikor_curl_handler_t *monikor_curl_handler_new(
  void (*callback)(monikor_curl_handler_t *handler, CURLcode result), void *data);

monikor_process_handler_t *monikor_process_handler_new(pid_t pid,
  void (*callback)(monikor_process_handler_t *handler, int status), void *data);

int monikor_io_handler_list_init(monikor_io_handler_list_t *list);
void monikor_io_handler_list_push(monikor_io_handler_list_t *list, monikor_io_handler_t *handler);
void monikor_io_handler_list_remove(monikor_io_handler_list_t *list, monikor_io_handler_t *handler);

void monikor_io_handler_list_push_curl(monikor_io_handler_list_t *list, monikor_curl_handler_t *handler);
void monikor_io_handler_list_remove_curl(monikor_io_handler_list_t *list, monikor_curl_handler_t *handler);

void monikor_io_handler_list_push_process(monikor_io_handler_list_t *list, monikor_process_handler_t *handler);
void monikor_io_handler_list_remove_process(monikor_io_handler_list_t *list, monikor_process_handler_t *handler);


int monikor_io_handler_poll(monikor_io_handler_list_t *list, struct timeval *timeout);

#endif /* end of include guard: MONIKOR_IO_HANDLER_H_ */
