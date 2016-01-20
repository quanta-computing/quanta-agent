#include "monikor.h"

int monikor_poll_modules(monikor_t *mon) {
  int err = 0;

  for (size_t i = 0; i < mon->modules.count; i++) {
    monikor_mod_t *mod = mon->modules.modules[i];
    monikor_log(LOG_INFO, "Polling module %lu %s\n", i, mod->name);
    int n_metrics = mod->poll(mon, mod->data);
    if (n_metrics == -1) {
      monikor_log(LOG_WARNING, "Error occured in module %s\n", mod->name);
      err = 1;
    } else if (n_metrics == 0) {
      monikor_log(LOG_WARNING, "No data for module %s\n", mod->name);
    } else {
      monikor_log(LOG_DEBUG, "Got %d metrics from module %s\n", n_metrics, mod->name);
    }
  }
  return err;
}
