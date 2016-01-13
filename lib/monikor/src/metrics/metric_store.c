#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "metric.h"

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

static inline int _timecmp(const struct timeval *a, const struct timeval *b) {
  if (a->tv_sec != b->tv_usec)
    return a->tv_sec - b->tv_sec;
  else
   return a->tv_usec - b->tv_usec;
}

static inline int is_cached_metric(monikor_metric_t *metric, monikor_metric_t *cached) {
  return (metric->id == cached->id)
    && !strcmp(cached->name, metric->name)
    && _timecmp(&metric->clock, &cached->clock);
}

/* Push a delta metric to the metric store.
** If the metric already exists, it is then computable and a new metric is returned. The metric
** already in place is replaced by the new one to allow further computation
*/
int monikor_metric_store_push_delta(monikor_metric_store_t *store, monikor_metric_t *metric) {
  for (monikor_metric_list_node_t *node = store->delta->first; node; node = node->next) {
    if (is_cached_metric(metric, node->metric)) {
      monikor_metric_t *new = monikor_metric_compute_delta(node->metric, metric);
      if (!new)
        return -1;
      monikor_metric_free(node->metric);
      node->metric = metric;
      return monikor_metric_store_push(store, new);
    }
  }
  return monikor_metric_list_push(store->delta, metric);
}

int monikor_metric_store_push_aggregate(monikor_metric_store_t *store, monikor_metric_t *metric) {
  for (monikor_metric_list_node_t *node = store->current->first; node; node = node->next) {
    if (!strcmp(node->metric->name, metric->name)
    && !_timecmp(&node->metric->clock, &metric->clock)
    && !monikor_metric_add(node->metric, metric)) {
      monikor_metric_free(metric);
      return 0;
    }
  }
  return monikor_metric_list_push(store->current, metric);
}

int monikor_metric_store_push(monikor_metric_store_t *store, monikor_metric_t *metric) {
  if (metric->flags & MONIKOR_METRIC_DELTA)
    return monikor_metric_store_push_delta(store, metric);
    else if (metric->flags & MONIKOR_METRIC_AGGREGATE)
      return monikor_metric_store_push_aggregate(store, metric);
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
