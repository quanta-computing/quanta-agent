#include <sys/time.h>
#include <curl/curl.h>

#include "monikor.h"

int monikor_init_modules(monikor_t *mon) {
  size_t mod_size;

  mod_size = (1 + mon->config->modules.modules->size) * sizeof(*mon->modules.modules);
  mon->modules.count = 0;
  if (!(mon->modules.modules = malloc(mod_size))
  || !(mon->metrics = monikor_metric_store_new())) {
    monikor_log(LOG_CRIT, "Cannot allocate memory\n");
    return 1;
  }
  if (monikor_load_all_modules(mon))
    return 1;
  if (monikor_info_module_load(mon))
    return 1;
  return 0;
}

int monikor_init(monikor_t *mon, char *config_path) {
  monikor_logger_init(MONIKOR_LOG_DEFAULT, NULL);
  if (!(mon->config = monikor_load_config(config_path)))
    return 1;
  monikor_logger_init(mon->config->logger.level, mon->config->logger.file);
  curl_global_init(CURL_GLOBAL_ALL);
  monikor_io_handler_list_init(&mon->io_handlers);
  mon->server.handler = NULL;
  mon->server.socket = -1;
  if (mon->config->listen.path) {
    if (monikor_server_init(&mon->server, mon))
      monikor_log(LOG_ERR, "Cannot start local server on %s\n", mon->config->listen.path);
  }
  if (monikor_daemonize(mon))
    return 1;
  if (monikor_register_signals(mon)) {
    monikor_log(LOG_CRIT, "Cannot register signal handlers\n");
    return 1;
  }
  if (monikor_init_modules(mon))
    return 1;
  mon->flags = 0;
  return 0;
}
