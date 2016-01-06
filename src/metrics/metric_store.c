#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "metric.h"
#include "logger.h"

monikor_metric_store_t *monikor_metric_store_new(void) {
  monikor_metric_store_t *store;

  if (!(store = malloc(sizeof(*store))))
    return NULL;
  store->current = NULL;
  store->delta = NULL;
  store->cache = NULL;
  if (!(store->current = monikor_metric_list_new())
  || !(store->delta = monikor_metric_list_new())
  || !(store->cache = monikor_metric_list_new())) {
    monikor_metric_store_free(store);
    return NULL;
  }
  return store;
}

void monikor_metric_store_free(monikor_metric_store_t *store) {
  if (!store)
    return;
  monikor_metric_list_free(store->current);
  monikor_metric_list_free(store->delta);
  monikor_metric_list_free(store->cache);
  free(store);
}

static inline int is_cached_metric(monikor_metric_t *metric, monikor_metric_t *cached) {
  return !strcmp(cached->name, metric->name)
    && (metric->clock.tv_sec != cached->clock.tv_sec
        || metric->clock.tv_usec != cached->clock.tv_usec);
}

/* Push a delta metric to the metric store.
** If the metric already exists, it is then computable and a new metric is returned. The metric
** already in place is replaced by the new one to allow further computation
*/
int monikor_metric_store_push_delta(monikor_metric_store_t *store, monikor_metric_t *metric) {
  for (monikor_metric_list_node_t *node = store->delta->first; node; node = node->next) {
    if (is_cached_metric(metric, node->metric)) {
      monikor_metric_t *new = monikor_metric_compute_delta(node->metric, metric);
      if (!new) {
        monikor_log(LOG_WARNING, "Cannot compute metric %s\n", metric->name);
        return -1;
      }
      monikor_metric_free(node->metric);
      node->metric = metric;
      return monikor_metric_list_push(store->current, new);
    }
  }
  return monikor_metric_list_push(store->delta, metric);
}

int monikor_metric_store_push(monikor_metric_store_t *store, monikor_metric_t *metric) {
  if (metric->flags & MONIKOR_METRIC_DELTA)
    return monikor_metric_store_push_delta(store, metric);
  else
    return monikor_metric_list_push(store->current, metric);
}

int monikor_metric_store_lpush(monikor_metric_store_t *store, monikor_metric_list_t *metrics) {
  monikor_metric_t *metric;
  int err = 0;

  while ((metric = monikor_metric_list_pop(metrics)))
    if (monikor_metric_store_push(store, metric))
      err = -1;
  return err;
}

void monikor_metric_store_flush(monikor_metric_store_t *store) {
  monikor_metric_list_empty(store->current);
}

void monikor_metric_store_cache(monikor_metric_store_t *store) {
  monikor_metric_list_concat(store->cache, store->current);
}
