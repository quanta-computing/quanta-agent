#include <stdlib.h>
#include <unistd.h>

#include "monikor.h"
#include "process.h"

void process_init(void) {
}

void process_cleanup(void) {
}


monikor_metric_list_t *process_poll(void) {
  monikor_metric_list_t *metrics;
  struct timeval now;

  gettimeofday(&now, NULL);
  if (!(metrics = monikor_metric_list_new()))
    return NULL;
  if (!poll_processes_metrics(metrics, &now)) {
    monikor_metric_list_free(metrics);
    return NULL;
  }
  return metrics;
}
