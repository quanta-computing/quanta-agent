#include <stdlib.h>
#include <time.h>

#include "monikor.h"
#include "system.h"


void *system_init(monikor_t *mon, monikor_config_dict_t *config) {
  (void)mon;
  (void)config;
  return NULL;
}

void system_cleanup(monikor_t *mon, void *data) {
  (void)mon;
  (void)data;
}

int system_poll(monikor_t *mon, void *data) {
  struct timeval now;

  (void)data;
  gettimeofday(&now, NULL);

  return system_poll_metrics(mon, &now);
}
