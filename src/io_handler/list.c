#include <stdlib.h>

#include "io_handler.h"

void monikor_io_handler_list_init(monikor_io_handler_list_t *list) {
  list->first = NULL;
  list->last = NULL;
}

void monikor_io_handler_list_push(monikor_io_handler_list_t *list, monikor_io_handler_t *handler) {
  if (!list->first) {
    list->first = handler;
    list->last = handler;
  } else {
    list->last->next = handler;
    handler->prev = list->last;
    list->last = handler;
  }
}

void monikor_io_handler_list_remove(monikor_io_handler_list_t *list, monikor_io_handler_t *handler) {
  if (handler->next)
    handler->next->prev = handler->prev;
  if (handler->prev)
    handler->prev->next = handler->next;
  if (handler == list->first)
    list->first = handler->next;
  if (handler == list->last)
    list->last = handler->prev;
}
