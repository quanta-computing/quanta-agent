#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "monikor.h"
#include "redis.h"

static const struct {
  char *field;
  char *name;
  uint8_t flags;
} metrics[] = {
  {"total_commands_processed", "requests_per_second", MONIKOR_METRIC_TIMEDELTA},
  {"keyspace_hits", "keyspace.hits", MONIKOR_METRIC_DELTA},
  {"keyspace_misses", "keyspace.misses", MONIKOR_METRIC_DELTA},
  {"evicted_keys", "keyspace.evictions", MONIKOR_METRIC_DELTA},
  {"used_memory", "size", 0},
  {"connected_clients", "connections.active", 0},
  {"blocked_clients", "connections.blocked", 0},
  {NULL, NULL, 0}
};

void redis_poll_metrics(char *status, void *data) {
  redis_module_t *mod = (redis_module_t *)data;
  char metric_name[256];
  struct timeval clock;
  char *metric;

  gettimeofday(&clock, NULL);
  for (size_t i = 0; metrics[i].name; i++) {
    sprintf(metric_name, "%s:", metrics[i].field);
    if ((metric = strstr(status, metric_name))) {
      metric += strlen(metric_name);
      if (mod->instance)
        sprintf(metric_name, "redis.%s.%s", metrics[i].name, mod->instance);
      else
        sprintf(metric_name, "redis.%s", metrics[i].name);
      monikor_metric_push(mod->mon, monikor_metric_integer(metric_name, &clock,
        (uint64_t)strtoull(metric, NULL, 10), metrics[i].flags));
    }
  }
}
