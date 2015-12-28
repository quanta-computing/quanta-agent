#include <stdlib.h>

#include "monikor.h"
#include "memory.h"

static const struct {
  char *name;
  char *field_name;
} memory_metrics[] = {
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

static int fetch_metric(const char *meminfo, const char *name, unsigned long *value) {
  char *line;

  if (!(line = strstr(meminfo, name)))
    return 1;
  *value = strtol(line + strlen(name), NULL, 10) * 1024;
  return 0;
}

int memory_poll_metrics(monikor_metric_list_t *metrics, time_t clock) {
  int n;
  char *meminfo;
  unsigned long value;

  if (!(meminfo = monikor_read_file("/proc/meminfo"))) {
    monikor_log_mod(LOG_WARNING, MOD_NAME, "Cannot read /proc/meminfo");
    return 0;
  }
  n = 0;
  for (size_t i = 0; memory_metrics[i].name; i++) {
    if (!fetch_metric(meminfo, memory_metrics[i].field_name, &value)) {
      monikor_metric_list_push(metrics, monikor_metric_integer(
        memory_metrics[i].name, clock, value
      ));
      n++;
    }
  }
  free(meminfo);
  return n;
}
