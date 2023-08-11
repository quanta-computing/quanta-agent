#include <stdlib.h>

#include "io_handler.h"

int monikor_io_handler_list_init(monikor_io_handler_list_t *list) {
  list->first = NULL;
  list->last = NULL;
  list->curl.first = NULL;
  list->curl.last = NULL;
  list->processes.first = NULL;
  list->processes.last = NULL;
  if (!(list->curl.curl = curl_multi_init()))
    return -1;
  return 0;
}

void monikor_io_handler_list_push(monikor_io_handler_list_t *list, monikor_io_handler_t *handler) {
  if (!list->first) {
    list->first = handler;
  } else {
    list->last->next = handler;
    handler->prev = list->last;
  }
  list->last = handler;
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

void monikor_io_handler_list_push_curl(monikor_io_handler_list_t *list, monikor_curl_handler_t *handler) {
  if (!list->curl.first) {
    list->curl.first = handler;
  } else {
    list->curl.last->next = handler;
    handler->prev = list->curl.last;
  }
  list->curl.last = handler;
  curl_multi_add_handle(list->curl.curl, handler->curl);
}

void monikor_io_handler_list_remove_curl(monikor_io_handler_list_t *list, monikor_curl_handler_t *handler) {
  if (handler->next)
    handler->next->prev = handler->prev;
  if (handler->prev)
    handler->prev->next = handler->next;
  if (handler == list->curl.first)
    list->curl.first = handler->next;
  if (handler == list->curl.last)
    list->curl.last = handler->prev;
  curl_multi_remove_handle(list->curl.curl, handler->curl);
}

void monikor_io_handler_list_push_process(monikor_io_handler_list_t *list, monikor_process_handler_t *handler) {
  if (!list->processes.first) {
    list->processes.first = handler;
  } else {
    list->processes.last->next = handler;
    handler->prev = list->processes.last;
  }
  list->processes.last = handler;
}

void monikor_io_handler_list_remove_process(monikor_io_handler_list_t *list, monikor_process_handler_t *handler) {
  if (handler->next)
    handler->next->prev = handler->prev;
  if (handler->prev)
    handler->prev->next = handler->next;
  if (handler == list->processes.first)
    list->processes.first = handler->next;
  if (handler == list->processes.last)
    list->processes.last = handler->prev;
}
