#include <stdlib.h>
#include <time.h>

#include "monikor.h"
#include "postfix.h"


void *postfix_init(monikor_t *mon, monikor_config_dict_t *config) {
  (void)mon;
  (void)config;
  return NULL;
}

void postfix_cleanup(monikor_t *mon, void *data) {
}

int postfix_poll(monikor_t *mon, void *data) {
  struct timeval now;

  gettimeofday(&now, NULL);
  return postfix_fetch_metrics(mon, &now);
}
