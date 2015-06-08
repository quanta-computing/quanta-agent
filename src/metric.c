#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "metric.h"


monikor_metric_t *monikor_metric_new(const char *name, time_t clock) {
  monikor_metric_t *metric;

  if (!(metric = malloc(sizeof(*metric))))
    return NULL;
  metric->name = strdup(name);
  metric->clock = clock;
  return metric;
}


monikor_metric_t *monikor_metric_integer(const char *name, time_t clock, long value) {
  monikor_metric_t *metric;

  if (!(metric = monikor_metric_new(name, clock)))
    return NULL;
  metric->type = MONIKOR_INTEGER;
  metric->value._int = value;
  return metric;
}


monikor_metric_t *monikor_metric_float(const char *name, time_t clock, float value) {
  monikor_metric_t *metric;

  if (!(metric = monikor_metric_new(name, clock)))
    return NULL;
  metric->type = MONIKOR_FLOAT;
  metric->value._float = value;
  return metric;
}


monikor_metric_t *monikor_metric_string(const char *name, time_t clock, const char *value) {
  monikor_metric_t *metric;

  if (!(metric = monikor_metric_new(name, clock)))
    return NULL;
  metric->type = MONIKOR_STRING;
  metric->value._string = strdup(value);
  return metric;
}


void monikor_metric_free(monikor_metric_t *metric) {
  if (metric->type == MONIKOR_STRING)
    free(metric->value._string);
  free(metric->name);
  free(metric);
}
