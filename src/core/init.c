#include "monikor.h"

int monikor_init(monikor_t *mon, const char *config_path) {
  size_t mod_size;

  mon->modules.count = 0;
  if (!(mon->config = monikor_load_config(config_path)))
    return 1;
  monikor_setup_config(mon->config);
  monikor_logger_init(mon->config->log_level);
  mod_size = mon->config->modules.modules->size * sizeof(*mon->modules.modules);
  mon->modules.count = 0;
  if (!(mon->modules.modules = malloc(mod_size))) {
    monikor_log(LOG_CRIT, "Cannot allocate memory\n");
    return 1;
  }
  return monikor_load_all_modules(mon);
}
