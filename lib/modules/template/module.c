#include <stdlib.h>
#include <time.h>

#include "monikor.h"
#include "__NAME__.h"


void *__NAME___init(monikor_t *mon, monikor_config_dict_t *config) {
  (void)mon;
  (void)config;
  return NULL;
}

void __NAME___cleanup(monikor_t *mon, void *data) {
}

int __NAME___poll(monikor_t *mon, void *data) {
  struct timeval now;

  gettimeofday(&now, NULL);
  return 0;
}
