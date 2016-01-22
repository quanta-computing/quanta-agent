#include <stdlib.h>

#include "monikor.h"

#include "memory.h"

void *memory_init(monikor_t *mon, monikor_config_dict_t *config) {
  (void)mon;
  (void)config;
  return NULL;
}


void memory_cleanup(monikor_t *mon, void *data) {
  (void)mon;
  (void)data;
}

int memory_poll(monikor_t *mon, void *data) {
  struct timeval now;

  (void)data;
  gettimeofday(&now, NULL);
  return memory_poll_metrics(mon, &now);
}
