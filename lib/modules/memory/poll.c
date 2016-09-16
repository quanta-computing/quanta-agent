#include <stdlib.h>
#include <errno.h>

#include "monikor.h"
#include "memory.h"

static const monikor_memory_metric_t meminfo_metrics[] = {
  {"memory.total", "MemTotal:"},
  {"memory.free", "MemFree:"},
  {"memory.available", "MemAvailable:"},
  {"memory.buffers", "Buffers:"},
  {"memory.cached", "Cached:"},
  {"memory.active", "Active:"},
  {"memory.inactive", "Inactive:"},
  {"swap.total", "SwapTotal:"},
  {"swap.free", "SwapFree:"},
  {NULL, NULL}
};

static const monikor_memory_metric_t vmstat_metrics[] = {
  {"swap.activity.pages_in", "pswpin"},
  {"swap.activity.pages_out", "pswpout"},
  {NULL, NULL}
};

static int fetch_metric(const char *info, const char *name, uint64_t *value) {
  char *line;

  if (!(line = strstr(info, name)))
    return 1;
  *value = strtoull(line + strlen(name), NULL, 10) * 1024;
  return 0;
}

static int fetch_metrics_from_file(monikor_t *mon, struct timeval *clock, const char *filename,
const monikor_memory_metric_t *metrics) {
  int n;
  char *info;
  uint64_t value;

  if (!(info = monikor_read_file(filename))) {
    monikor_log_mod(LOG_WARNING, MOD_NAME,
      "Cannot read %s: %s", filename, strerror(errno));
    return 0;
  }
  n = 0;
  for (size_t i = 0; metrics[i].name; i++) {
    if (!fetch_metric(info, metrics[i].field_name, &value)) {
      monikor_metric_push(mon, monikor_metric_integer(
        metrics[i].name, clock, value, 0
      ));
      n++;
    }
  }
  free(info);
  return n;
}

int memory_poll_metrics(monikor_t *mon, struct timeval *clock) {
  return fetch_metrics_from_file(mon, clock, "/proc/meminfo", meminfo_metrics)
    + fetch_metrics_from_file(mon, clock, "/proc/vmstat", vmstat_metrics);
}
