#include "monikor.h"

/*
** Reload configuration and modules.
** We cannot reload the server and other stuff since we are already in unprivilegied mode
*/
int monikor_reload(monikor_t *mon) {
  monikor_config_t *config;

  monikor_log(LOG_DEBUG, "Reloading configuration and modules\n");
  if (!(config = monikor_load_config(mon->config->config_path))) {
    monikor_log(LOG_ERR, "Cannot reload configuration, keeping the current one\n");
    mon->flags &= ~MONIKOR_FLAG_RELOAD;
    return 0;
  }
  // Keep module libraries mapped: in-flight async callbacks (http, command) still point into them
  for (size_t i = 0; i < mon->modules.count; i++)
    mon->modules.modules[i]->dhandle = NULL;
  monikor_cleanup_modules(mon);
  monikor_config_free(mon->config);
  mon->config = config;
  monikor_logger_init(mon->config->logger.level, mon->config->logger.file);
  if (monikor_init_modules(mon))
    return 1;
  mon->flags &= ~MONIKOR_FLAG_RELOAD;
  return 0;
}
