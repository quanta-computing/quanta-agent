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


static void poll_base_metrics(const char *status, redis_module_t *mod, struct timeval *clock) {
  char metric_name[256];
  char *metric;

  for (size_t i = 0; metrics[i].name; i++) {
    sprintf(metric_name, "%s:", metrics[i].field);
    if ((metric = strstr(status, metric_name))) {
      metric += strlen(metric_name);
      if (mod->instance)
        sprintf(metric_name, "redis.%s.%s", metrics[i].name, mod->instance);
      else
        sprintf(metric_name, "redis.%s", metrics[i].name);
      monikor_metric_push(mod->mon, monikor_metric_integer(metric_name, clock,
        (uint64_t)strtoull(metric, NULL, 10), metrics[i].flags));
    }
  }
}

static void poll_keys_metrics(const char *status, redis_module_t *mod, struct timeval *clock) {
  char metric_name[256];
  char *metric;
  uint64_t count = 0;

  if (!(metric = strstr(status, "# Keyspace")))
    return;
  while ((metric = strstr(metric, ":keys="))) {
    metric += strlen(":keys=");
    count += (uint64_t)strtoull(metric, NULL, 0);
  }
  if (mod->instance)
    sprintf(metric_name, "redis.keyspace.keys.%s", mod->instance);
  else
    strcpy(metric_name, "redis.keyspace.keys");
  monikor_metric_push(mod->mon, monikor_metric_integer(
    metric_name, clock, count, 0));
}

void redis_poll_metrics(char *status, void *data) {
  redis_module_t *mod = (redis_module_t *)data;
  struct timeval clock;

  gettimeofday(&clock, NULL);
  poll_base_metrics(status, mod, &clock);
  poll_keys_metrics(status, mod, &clock);
}
