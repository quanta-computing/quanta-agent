#include <stdlib.h>
#include <time.h>

#include "monikor.h"
#include "network.h"


void network_init(void) {
}

void network_cleanup(void) {
}

monikor_metric_list_t *network_poll(void) {
  monikor_metric_list_t *metrics;
  struct timeval now;

  gettimeofday(&now, NULL);
  if (!(metrics = monikor_metric_list_new()))
    return NULL;
  if (!poll_network_metrics(metrics, &now)) {
    monikor_metric_list_free(metrics);
    return NULL;
  }
  return metrics;
}
