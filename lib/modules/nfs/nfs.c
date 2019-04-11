#include <stdlib.h>
#include <time.h>

#include "monikor.h"
#include "nfs.h"


void *nfs_init(monikor_t *mon, monikor_config_dict_t *config) {
  (void)mon;
  (void)config;
  return NULL;
}

void nfs_cleanup(monikor_t *mon, void *data) {
  (void)mon;
  (void)data;
}

int nfs_poll(monikor_t *mon, void *data) {
  struct timeval now;

  (void)data;
  gettimeofday(&now, NULL);
  return nfs_fetch_metrics(mon, &now);
}
