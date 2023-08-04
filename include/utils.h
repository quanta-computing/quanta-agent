#ifndef MONIKOR_UTILS_H_H
#define MONIKOR_UTILS_H_H

#include <stdlib.h>
#include <stdint.h>
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

char *monikor_read_file(const char *filepath);
char *monikor_read_fd(int fd);
int monikor_http_get(monikor_t *mon, const char *url, long timeout,
  void (*callback)(http_response_t *response, CURLcode result), void *data);
int monikor_http_get_with_headers(monikor_t *mon, const char *url, long timeout,
  void (*callback)(http_response_t *response, CURLcode result),
  void *data, struct curl_slist *headers);
monikor_io_handler_t *monikor_net_exchange(const char *host, const char *port, char *send,
void (*callback)(char *response, void *data), void *data);

// DEBUG stuff
void dump_metric_list(monikor_metric_list_t *list);
void dump_store_size(monikor_metric_store_t *store);

#endif /* end of include guard: MONIKOR_UTILS_H_H */
