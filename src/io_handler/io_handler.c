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
