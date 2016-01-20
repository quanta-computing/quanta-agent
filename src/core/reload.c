#include "monikor.h"

int monikor_reload(monikor_t *mon) {
  char *config_path;

  monikor_log(LOG_DEBUG, "Reloading configuration and modules\n");
  config_path = mon->config->config_path;
  monikor_cleanup(mon);
  mon->flags &= ~MONIKOR_FLAG_RELOAD;
  return monikor_init(mon, config_path);
}
