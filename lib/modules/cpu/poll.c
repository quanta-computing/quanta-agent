#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "monikor.h"
#include "cpu.h"

static const char *metric_names[MNK_CPU_NB_METRICS] = {
  "cpu.user", "cpu.nice", "cpu.system", "cpu.idle",
  "cpu.iowait", "cpu.irq", "cpu.softirq", "cpu.steal",
  "cpu.guest"
};


char *get_proc_stat_cpu_info(char *proc_stat) {
  char *line;
  char *eol;

  if (!(line = strstr(proc_stat, "cpu "))
  || !(eol = strchr(line, '\n')))
    return NULL;
  *eol = 0;
  return line;
}

static size_t cpu_fetch_metrics(uint64_t *values, size_t n_metrics) {
  char *proc_stat;
  char *cpu_info;
  char *start;
  char *end;
  unsigned i;

  if (!(proc_stat = monikor_read_file("/proc/stat"))
  || !(cpu_info = get_proc_stat_cpu_info(proc_stat)))
    return 0;
  start = end = strchr(cpu_info, ' ');
  for (i = 0; i < n_metrics && end; i++) {
    for (; *start == ' '; start++);
    end = strchr(start, ' ');
    if (end)
      *end = 0;
    values[i] = strtoull(start, NULL, 10);
    start = end + 1;
  }
  free(proc_stat);
  return i;
}

static int cpu_fetch_loadavg(monikor_t *mon, struct timeval *clock) {
  float value;
  char *loadavg;
  char *end;

  if (!(loadavg = monikor_read_file("/proc/loadavg")))
    return 1;
  value = strtof(loadavg, &end);
  free(loadavg);
  if (end == loadavg)
    return 1;
  monikor_metric_push(mon, monikor_metric_float("cpu.loadavg",
    clock, value / sysconf(_SC_NPROCESSORS_ONLN), 0));
  return 0;
}

static double cpu_compute_interval(const struct timeval *a, const struct timeval *b) {
  return (b->tv_sec - a->tv_sec) + (b->tv_usec - a->tv_usec) / 1000000.0;
}

// Deltas on the raw 64-bit jiffies: floats of cumulative seconds lose precision with uptime
static void cpu_push_deltas(monikor_t *mon, struct timeval *clock, monikor_cpu_mod_t *mod,
const uint64_t *values, size_t n) {
  double interval;
  double ticks;

  if (!mod->n_jiffies)
    return;
  interval = cpu_compute_interval(&mod->clock, clock);
  ticks = (double)sysconf(_SC_CLK_TCK) * sysconf(_SC_NPROCESSORS_ONLN) * interval;
  if (interval <= 0 || interval > 3 * mon->config->poll_interval || ticks <= 0)
    return;
  for (size_t i = 0; i < n && i < mod->n_jiffies; i++) {
    monikor_metric_push(mon, monikor_metric_float(metric_names[i], clock,
      values[i] < mod->jiffies[i] ? 0.0 : (values[i] - mod->jiffies[i]) / ticks, 0
    ));
  }
}

int cpu_poll_metrics(monikor_t *mon, struct timeval *clock, monikor_cpu_mod_t *mod) {
  uint64_t values[MNK_CPU_NB_METRICS];
  int n;

  n = cpu_fetch_metrics(values, MNK_CPU_NB_METRICS);
  if (n) {
    cpu_push_deltas(mon, clock, mod, values, n);
    memcpy(mod->jiffies, values, n * sizeof(*values));
    mod->n_jiffies = n;
    mod->clock = *clock;
  }
  if (!cpu_fetch_loadavg(mon, clock))
    n++;
  return n;
}
