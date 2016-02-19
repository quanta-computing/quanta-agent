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
  monikor_curl_handler_t *curl_cur;
  monikor_curl_handler_t *curl_next;

  for (cur = mon->io_handlers.first; cur; cur = next) {
    next = cur->next;
    close(cur->fd);
    free(cur);
  }
  for (curl_cur = mon->io_handlers.curl.first; curl_cur; curl_cur = curl_next) {
    curl_next = curl_cur->next;
    curl_multi_remove_handle(mon->io_handlers.curl.curl, curl_cur->curl);
    curl_easy_cleanup(curl_cur->curl);
    free(curl_cur);
  }
  curl_multi_cleanup(mon->io_handlers.curl.curl);
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
