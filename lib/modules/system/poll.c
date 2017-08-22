#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <inttypes.h>

#include "monikor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

static const struct {
  char *stat_name;
  char *name;
} metrics[] = {
  {"intr ", "system.interrupts"},
  {"ctxt ", "system.context_switches"},
  {"processes ", "system.forks"},
  {NULL, NULL}
};

int poll_file(monikor_t *mon, struct timeval *clock){
  int n = 0;
  char *stat;
  char *str;
  uint64_t value;

  if (!(stat = monikor_read_file("/proc/sys/fs/file-nr")))
    return 0;
  value = strtoull(stat, NULL, 0);
  n += monikor_metric_push(mon, monikor_metric_integer(
    "system.file.open", clock, value, 0));
  if (!(str = strrchr(stat, '\t')))
    goto err;
  value =  strtoull(str, NULL, 0);
  n += monikor_metric_push(mon, monikor_metric_integer(
    "system.file.max", clock, value, 0));

err:
  free(stat);
  return n;
}

int poll_inode(monikor_t *mon, struct timeval *clock){
  int n = 0;
  char *stat;
  uint64_t value;

  if (!(stat = monikor_read_file("/proc/sys/fs/inode-nr")))
    return 0;
  value = strtoull(stat, NULL, 0);
  n += monikor_metric_push(mon, monikor_metric_integer(
    "system.inode.open", clock, value, 0));
  free(stat);
  return n;
}

int poll_metric(const char *name, const char *stat, uint64_t *value) {
  char *metric;

  if (!(metric = strstr(stat, name)))
    return 1;
  metric += strlen(name);
  *value = strtoull(metric, NULL, 0);
  return 0;
}


int system_poll_metrics(monikor_t *mon, struct timeval *clock) {
  uint64_t value;
  char *stat;
  int n = 0;

  if (!(stat = monikor_read_file("/proc/stat")))
    return 0;
  for (size_t i = 0; metrics[i].name; i++) {
    if (poll_metric(metrics[i].stat_name, stat, &value))
      continue;
    // printf("METRIC %s: %" PRIu64 "\n", metrics[i].name, value);
    n += monikor_metric_push(mon, monikor_metric_integer(
      metrics[i].name, clock, value, MONIKOR_METRIC_TIMEDELTA));
  }
  n += poll_inode(mon, clock);
  n += poll_file(mon, clock);
  free(stat);
  return n;
}
