#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "monikor.h"
#include "memcached.h"

static const struct {
  char *hit;
  char *miss;
} hitmiss_metrics[] = {
  {"get_hits", "get_misses"},
  {"delete_hits", "delete_misses"},
  {"incr_hits", "incr_misses"},
  {"decr_hits", "decr_misses"},
  {"cas_hits", "cas_misses"},
  {"touch_hits", "touch_misses"},
  {NULL, NULL}
};

static void fetch_hitmiss_metrics(memcached_module_t *mod, struct timeval *clock, char *status) {
  char metric_name[256];
  char *metric;
  uint64_t hits = 0;
  uint64_t misses = 0;

  for (size_t i = 0; hitmiss_metrics[i].hit; i++) {
    sprintf(metric_name, "STAT %s ", hitmiss_metrics[i].hit);
    if ((metric = strstr(status, metric_name))) {
      metric += strlen(metric_name);
      hits += strtoull(metric, NULL, 10);
    }
    sprintf(metric_name, "STAT %s ", hitmiss_metrics[i].miss);
    if ((metric = strstr(status, metric_name))) {
      metric += strlen(metric_name);
      misses += strtoull(metric, NULL, 10);
    }
  }
  if (mod->instance)
    sprintf(metric_name, "memcached.cache.hits.%s", mod->instance);
  else
    sprintf(metric_name, "memcached.cache.hits");
  monikor_metric_push(mod->mon, monikor_metric_integer(
    metric_name, clock, hits, MONIKOR_METRIC_DELTA));
  if (mod->instance)
    sprintf(metric_name, "memcached.cache.misses.%s", mod->instance);
  else
    sprintf(metric_name, "memcached.cache.misses");
  monikor_metric_push(mod->mon, monikor_metric_integer(
    metric_name, clock, misses, MONIKOR_METRIC_DELTA));
}

static const struct {
  char *field;
  char *name;
  uint8_t flags;
} metrics[] = {
  {"cmd_get", "queries.get", MONIKOR_METRIC_TIMEDELTA},
  {"cmd_set", "queries.set", MONIKOR_METRIC_TIMEDELTA},
  {"cmd_flush", "queries.flush", MONIKOR_METRIC_TIMEDELTA},
  {"cmd_touch", "queries.touch", MONIKOR_METRIC_TIMEDELTA},
  {"bytes_read", "rx.bytes", MONIKOR_METRIC_TIMEDELTA},
  {"bytes_written", "tx.bytes", MONIKOR_METRIC_TIMEDELTA},
  {"curr_connections", "connections.current", 0},
  {"threads", "threads", 0},
  {"bytes", "size", 0},
  {"evictions", "cache.evictions", MONIKOR_METRIC_DELTA},
  {NULL, NULL, 0}
};

void memcached_poll_metrics(char *status, void *data) {
  memcached_module_t *mod = (memcached_module_t *)data;
  char metric_name[256];
  struct timeval clock;
  char *metric;

  gettimeofday(&clock, NULL);
  for (size_t i = 0; metrics[i].name; i++) {
    sprintf(metric_name, "STAT %s ", metrics[i].field);
    if ((metric = strstr(status, metric_name))) {
      metric += strlen(metric_name);
      if (mod->instance)
        sprintf(metric_name, "memcached.%s.%s", metrics[i].name, mod->instance);
      else
        sprintf(metric_name, "memcached.%s", metrics[i].name);
      monikor_metric_push(mod->mon, monikor_metric_integer(metric_name, &clock,
        (uint64_t)strtoull(metric, NULL, 10), metrics[i].flags));
    }
  }
  fetch_hitmiss_metrics(mod, &clock, status);
}
