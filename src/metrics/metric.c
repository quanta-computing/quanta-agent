#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "metric.h"


monikor_metric_t *monikor_metric_new(const char *name, const struct timeval *clock) {
  monikor_metric_t *metric;

  if (!(metric = malloc(sizeof(*metric))))
    return NULL;
  metric->name = strdup(name);
  metric->id = 0;
  metric->clock.tv_sec = clock->tv_sec;
  metric->clock.tv_usec = clock->tv_usec;
  metric->flags = 0;
  return metric;
}

monikor_metric_t *monikor_metric_clone(const monikor_metric_t *src) {
  monikor_metric_t *metric;

  if (!(metric = monikor_metric_new(src->name, &src->clock)))
    return NULL;
  metric->id = src->id;
  metric->flags = src->flags;
  metric->type = src->type;
  return metric;
}

void monikor_metric_free(monikor_metric_t *metric) {
  if (metric->type == MONIKOR_STRING)
    free(metric->value._string);
  free(metric->name);
  free(metric);
}

monikor_metric_t *monikor_metric_integer(const char *name, const struct timeval *clock,
uint64_t value, uint8_t flags) {
  monikor_metric_t *metric;

  if (!(metric = monikor_metric_new(name, clock)))
    return NULL;
  metric->type = MONIKOR_INTEGER;
  metric->value._int = value;
  metric->flags = flags;
  return metric;
}

monikor_metric_t *monikor_metric_integer_id(const char *name, const struct timeval *clock,
uint64_t value, uint8_t flags, uint16_t id) {
  monikor_metric_t *metric;

  if (!(metric = monikor_metric_integer(name, clock, value, flags)))
    return NULL;
  metric->id = id;
  return metric;
}

monikor_metric_t *monikor_metric_float(const char *name, const struct timeval *clock,
float value, uint8_t flags) {
  monikor_metric_t *metric;

  if (!(metric = monikor_metric_new(name, clock)))
    return NULL;
  metric->type = MONIKOR_FLOAT;
  metric->value._float = value;
  metric->flags = flags;
  return metric;
}

monikor_metric_t *monikor_metric_float_id(const char *name, const struct timeval *clock,
float value, uint8_t flags, uint16_t id) {
  monikor_metric_t *metric;

  if (!(metric = monikor_metric_float(name, clock, value, flags)))
    return NULL;
  metric->id = id;
  return metric;
}

monikor_metric_t *monikor_metric_string(const char *name, const struct timeval *clock,
const char *value) {
  monikor_metric_t *metric;

  if (!(metric = monikor_metric_new(name, clock)))
    return NULL;
  metric->type = MONIKOR_STRING;
  metric->value._string = strdup(value);
  return metric;
}
