#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>

#include "monikor.h"
#include "server.h"

static void cleanup_modules(monikor_t *mon) {
  for (size_t i = 0; i < mon->modules.count; i++)
    monikor_mod_destroy(mon, mon->modules.modules[i]);
  free(mon->modules.modules);
}

static void cleanup_io_handlers(monikor_t *mon) {
  monikor_io_handler_t *cur;
  monikor_io_handler_t *next;

  for (cur = mon->io_handlers.first; cur; cur = next) {
    next = cur->next;
    close(cur->fd);
    free(cur);
  }
}

void monikor_cleanup(monikor_t *mon) {
  monikor_server_cleanup(&mon->server);
  cleanup_modules(mon);
  cleanup_io_handlers(mon);
  monikor_metric_store_free(mon->metrics);
  monikor_config_free(mon->config);
  monikor_logger_cleanup();
  monikor_signal_cleanup();
  curl_global_cleanup();
}
