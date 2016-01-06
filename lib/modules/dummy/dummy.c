#include <stdio.h>
#include <time.h>
#include "monikor.h"

#define MOD_NAME "dummy"


void dummy_init(void) {
  monikor_log_mod(LOG_INFO, MOD_NAME, "Dummy init\n");
}

void dummy_cleanup(void) {
  monikor_log_mod(LOG_INFO, MOD_NAME, "Dummy cleanup\n");
}

monikor_metric_list_t *dummy_poll(void) {
  monikor_metric_list_t *metrics;
  struct timeval now;

  gettimeofday(&now, NULL);
  monikor_log_mod(LOG_INFO, MOD_NAME, "Dummy poll\n");
  if (!(metrics = monikor_metric_list_new()))
    return NULL;
  monikor_metric_list_push(metrics, monikor_metric_integer("toto", &now, 42, 0));
  monikor_metric_list_push(metrics, monikor_metric_string("tata", &now, "awesome"));
  return metrics;
}
