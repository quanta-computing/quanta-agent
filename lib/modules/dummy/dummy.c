#include <stdio.h>
#include <time.h>
#include "monikor.h"

#define MOD_NAME "dummy"


void *dummy_init(monikor_t *mon) {
  (void)mon;
  monikor_log_mod(LOG_INFO, MOD_NAME, "Dummy init\n");
  return (void *)"toto";
}

void dummy_cleanup(monikor_t *mon, void *data) {
  (void)mon;
  monikor_log_mod(LOG_INFO, MOD_NAME, "Dummy cleanup %s\n", (char *)data);
}

int dummy_poll(monikor_t *mon, void *data) {
  struct timeval now;
  char name[512];
  int n = 0;

  gettimeofday(&now, NULL);
  monikor_log_mod(LOG_INFO, MOD_NAME, "Dummy poll %s\n", (char *)data);
  for (size_t i = 0; i < 20000; i++) {
    sprintf(name, "dummy.%zu", i);
    n += monikor_metric_push(mon, monikor_metric_integer(name, &now, 42, 0));
  }
  return n;
}
