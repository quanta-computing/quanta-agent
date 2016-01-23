#ifndef MONIKOR_UTILS_H_H
#define MONIKOR_UTILS_H_H

#include <stdlib.h>
#include <stdint.h>

#include "io_handler.h"

typedef struct {
  long code;
  char *data;
  size_t size;
} http_response_t;

char *monikor_read_file(const char *filepath);
http_response_t *monikor_http_get(const char *url, long timeout);

monikor_io_handler_t *monikor_net_exchange(const char *host, const char *port, char *send,
void (*callback)(char *response, void *data), void *data);

int timecmp(const struct timeval *a, const struct timeval *b);

// DEBUG stuff
void dump_metric_list(monikor_metric_list_t *list);
void dump_store_size(monikor_metric_store_t *store);

#endif /* end of include guard: MONIKOR_UTILS_H_H */
