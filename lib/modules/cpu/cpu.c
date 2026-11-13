#include <stdlib.h>
#include <time.h>

#include "monikor.h"
#include "cpu.h"

void *cpu_init(monikor_t *mon, monikor_config_t *config) {
  monikor_cpu_mod_t *mod;

  (void)mon;
  (void)config;
  if (!(mod = malloc(sizeof(*mod))))
    return NULL;
  mod->n_jiffies = 0;
  return mod;
}

void cpu_cleanup(monikor_t *mon, void *data) {
  (void)mon;
  free(data);
}

int cpu_poll(monikor_t *mon, void *data) {
  monikor_cpu_mod_t *mod = (monikor_cpu_mod_t *)data;
  struct timeval now;

  if (!mod) {
    monikor_log_mod(LOG_ERR, MOD_NAME, "Invalid context data.\n");
    return -1;
  }
  gettimeofday(&now, NULL);
  return cpu_poll_metrics(mon, &now, mod);
}
