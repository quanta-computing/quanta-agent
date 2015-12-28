#include <stdlib.h>
#include <time.h>
#include "monikor.h"

#include "memory.h"

void memory_init(void) {
}


void memory_cleanup(void) {
}

monikor_metric_list_t *memory_poll(void) {
  monikor_metric_list_t *metrics;
  time_t now = time(NULL);

  if (!(metrics = monikor_metric_list_new()))
    return NULL;
  if (!memory_poll_metrics(metrics, now)) {
    monikor_metric_list_free(metrics);
    return NULL;
  }
  return metrics;
}
