#include "monikor.h"

int monikor_metric_push(monikor_t *mon, monikor_metric_t *metric) {
  if (!metric)
    return 0;
  monikor_metric_store_push(mon->metrics, metric);
  return 1;
}
