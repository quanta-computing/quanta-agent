#include <stdlib.h>
#include <time.h>

#include "monikor.h"
#include "network.h"


void *network_init(monikor_t *mon, monikor_config_dict_t *config) {
  (void)mon;
  (void)config;
  return NULL;
}

void network_cleanup(monikor_t *mon, void *data) {
  (void)mon;
  (void)data;
}

int network_poll(monikor_t *mon, void *data) {
  struct timeval now;
  int i = 0;

  (void)data;
  gettimeofday(&now, NULL);
  i += poll_network_metrics(mon, &now);
  i += poll_tcp_metrics(mon, &now);
  return i;
}
