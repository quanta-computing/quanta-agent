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
  if (a->tv_sec != b->tv_sec)
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

size_t monikor_metric_store_evict_cache(monikor_metric_store_t *store, size_t max_size) {
  size_t n;

  for (n = 0; store->cache->data_size > max_size; n++)
    monikor_metric_free(monikor_metric_list_pop(store->cache));
  return n;
}

static int evict_delta_predicate(monikor_metric_t *metric, void *data) {
  return _timecmp(&metric->clock, (struct timeval *)data) < 0;
}

size_t monikor_metric_store_evict_delta(monikor_metric_store_t *store, struct timeval *clock) {
  return monikor_metric_list_remove_if(store->delta, &evict_delta_predicate, (void *)clock);
}

void monikor_metric_store_flush(monikor_metric_store_t *store) {
  monikor_metric_list_empty(store->current);
}

void monikor_metric_store_cache(monikor_metric_store_t *store) {
  monikor_metric_list_concat(store->cache, store->current);
}

/*
** We perform 2 concat operations to keep the cache data at the beginning of the list.
** This allows older data to be evicted first when evicting the cache.
*/
void monikor_metric_store_uncache(monikor_metric_store_t *store) {
  monikor_metric_list_concat(store->cache, store->current);
  monikor_metric_list_concat(store->current, store->cache);
}

void monikor_metric_store_flush_all(monikor_metric_store_t *store) {
  monikor_metric_list_empty(store->current);
  monikor_metric_list_empty(store->delta);
  monikor_metric_list_empty(store->cache);
}
