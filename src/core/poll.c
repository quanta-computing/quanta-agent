#include <sys/time.h>

#include "monikor.h"

static int poll_module(monikor_t *mon, monikor_mod_t *mod) {
  int n_metrics;

  monikor_log(LOG_INFO, "Polling module %s\n", mod->name);
  n_metrics = mod->poll(mon, mod->data);
  if (n_metrics == -1) {
    monikor_log(LOG_WARNING, "Error occured when polling module %s\n", mod->name);
    return 1;
  } else if (n_metrics == 0) {
    monikor_log(LOG_WARNING, "No data for module %s\n", mod->name);
  } else if (n_metrics == MONIKOR_MOD_DEFERRED_METRICS_CODE) {
    monikor_log(LOG_DEBUG, "Metrics fetch deferred for module %s\n", mod->name);
  } else {
    monikor_log(LOG_DEBUG, "Got %d metrics from module %s\n", n_metrics, mod->name);
  }
  return 0;
}

int monikor_poll_modules(monikor_t *mon, struct timeval *clock) {
  int err = 0;

  for (size_t i = 0; i < mon->modules.count; i++) {
    time_t next_update = mon->modules.modules[i]->last_clock.tv_sec
      + mon->modules.modules[i]->poll_interval;
    if (next_update <= clock->tv_sec) {
      err += poll_module(mon, mon->modules.modules[i]);
      mon->modules.modules[i]->last_clock.tv_sec = next_update;
    }
  }
  return err;
}
