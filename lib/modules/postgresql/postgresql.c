#include <stdlib.h>
#include <time.h>

#include "monikor.h"
#include "postgresql.h"


void *postgresql_init(monikor_t *mon, monikor_config_dict_t *config) {
  (void)mon;
  (void)config;
  return NULL;
}

void postgresql_cleanup(monikor_t *mon, void *data) {
  (void)mon;
  (void)data;
}

int postgresql_poll(monikor_t *mon, void *data) {
  struct timeval now;

  (void)data;
  gettimeofday(&now, NULL);
  return (postgresql_poll_metrics(mon, n));
}
