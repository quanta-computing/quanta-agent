#include <stdlib.h>
#include <time.h>

#include "monikor.h"
#include "disk.h"


void *disk_init(monikor_t *mon, monikor_config_dict_t *config) {
  (void)mon;
  (void)config;
  return NULL;
}

void disk_cleanup(monikor_t *mon, void *data) {
  (void)mon;
  (void)data;
}

int disk_poll(monikor_t *mon, void *data) {
  struct timeval now;

  (void)data;
  gettimeofday(&now, NULL);
  return poll_disk_metrics(mon, &now);
}
