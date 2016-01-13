#include "monikor.h"
#include "io_handler.h"

void monikor_register_io_handler(monikor_t *mon, monikor_io_handler_t *handler) {
  monikor_io_handler_list_push(&mon->io_handlers, handler);
}

void monikor_unregister_io_handler(monikor_t *mon, monikor_io_handler_t *handler) {
  monikor_io_handler_list_remove(&mon->io_handlers, handler);
}
