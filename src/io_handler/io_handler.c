#include <stdlib.h>
#include <stdint.h>

#include "io_handler.h"

monikor_io_handler_t *monikor_io_handler_new(int fd, uint8_t mode,
void (*callback)(monikor_io_handler_t *handler, uint8_t mode), void *data) {
  monikor_io_handler_t *handler;

  if (!(handler = malloc(sizeof(*handler))))
    return NULL;
  handler->fd = fd;
  handler->mode = mode;
  handler->callback = callback;
  handler->data = data;
  handler->next = NULL;
  handler->prev = NULL;
  return handler;
}

monikor_curl_handler_t *monikor_curl_handler_new(
void (*callback)(monikor_curl_handler_t *handler, CURLcode result), void *data) {
  monikor_curl_handler_t *handler;

  if (!(handler = malloc(sizeof(*handler)))
  || !(handler->curl = curl_easy_init())) {
    free(handler);
    return NULL;
  }
  handler->callback = callback;
  handler->data = data;
  handler->next = NULL;
  handler->prev = NULL;
  return handler;
}
