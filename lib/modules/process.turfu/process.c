#include <stdlib.h>
#include <unistd.h>

#include "monikor.h"
#include "process.h"

void *process_init(monikor_t *mon, monikor_config_dict_t *config) {
  (void)mon;
  (void)config;
  return NULL;
}

void process_cleanup(monikor_t *mon, void *data) {
  (void)mon;
  (void)data;
}

int process_poll(monikor_t *mon, void *data) {
  struct timeval now;

  (void)data;
  gettimeofday(&now, NULL);
  return poll_processes_metrics(mon, &now);
}
