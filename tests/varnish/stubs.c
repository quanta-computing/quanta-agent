#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "monikor.h"
#include "stubs.h"

recorded_metric_t recorded_metrics[RECORDED_METRICS_MAX];
size_t recorded_metrics_count = 0;

void recorded_metrics_reset(void) {
  recorded_metrics_count = 0;
}

const recorded_metric_t *recorded_metric_find(const char *name) {
  for (size_t i = 0; i < recorded_metrics_count; i++)
    if (!strcmp(recorded_metrics[i].name, name))
      return &recorded_metrics[i];
  return NULL;
}

int monikor_metric_push(monikor_t *mon, monikor_metric_t *metric) {
  (void)mon;
  if (!metric)
    return 0;
  if (recorded_metrics_count < RECORDED_METRICS_MAX) {
    recorded_metric_t *rec = &recorded_metrics[recorded_metrics_count++];
    snprintf(rec->name, sizeof(rec->name), "%s", metric->name);
    rec->value = metric->value._int;
    rec->flags = metric->flags;
  }
  monikor_metric_free(metric);
  return 1;
}

int monikor_log_mod(int prio, const char *mod_name, const char *message, ...) {
  va_list ap;

  if (!getenv("VARNISH_TEST_VERBOSE"))
    return 0;
  fprintf(stderr, "[%d] %s: ", prio, mod_name);
  va_start(ap, message);
  vfprintf(stderr, message, ap);
  va_end(ap);
  return 0;
}
