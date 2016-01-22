#include <time.h>

#include "monikor.h"
#include "cpu.h"

void *cpu_init(monikor_t *mon, monikor_config_t *config) {
  (void)mon;
  (void)config;
  return NULL;
}

void cpu_cleanup(monikor_t *mon, void *data) {
  (void)mon;
  (void)data;
}

int cpu_poll(monikor_t *mon, void *data) {
  struct timeval now;

  (void)data;
  gettimeofday(&now, NULL);
  return cpu_poll_metrics(mon, &now);
}
