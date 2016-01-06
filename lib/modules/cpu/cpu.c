#include <time.h>

#include "monikor.h"
#include "cpu.h"

void cpu_init(void) {
}

void cpu_cleanup(void) {
}

monikor_metric_list_t *cpu_poll(void) {
  monikor_metric_list_t *metrics;
  struct timeval now;

  gettimeofday(&now, NULL);
  if (!(metrics = monikor_metric_list_new())
  || !cpu_poll_metrics(metrics, &now)) {
    monikor_metric_list_free(metrics);
    return NULL;
  }
  return metrics;
}
