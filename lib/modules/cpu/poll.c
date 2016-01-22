#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "monikor.h"
#include "cpu.h"

static const char *metric_names[] = {
  "cpu.user", "cpu.nice", "cpu.system", "cpu.idle",
  "cpu.iowait", "cpu.irq", "cpu.softirq", "cpu.steal",
  "cpu.guest"
};
#define NB_CPU_METRICS (sizeof(metric_names) / sizeof(*metric_names))


char *get_proc_stat_cpu_info(char *proc_stat) {
  char *line;
  char *eol;

  if (!(line = strstr(proc_stat, "cpu "))
  || !(eol = strchr(line, '\n')))
    return NULL;
  *eol = 0;
  return line;
}

static size_t cpu_fetch_metrics(float *values, size_t n_metrics) {
  char *proc_stat;
  char *cpu_info;
  char *start;
  char *end;
  unsigned i;
  long tps = sysconf (_SC_CLK_TCK);

  if (!(proc_stat = monikor_read_file("/proc/stat"))
  || !(cpu_info = get_proc_stat_cpu_info(proc_stat)))
    return 0;
  start = end = strchr(cpu_info, ' ');
  for (i = 0; i < n_metrics && end; i++) {
    for (; *start == ' '; start++);
    end = strchr(start, ' ');
    if (end)
      *end = 0;
    values[i] = strtof(start, NULL) / tps;
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

int cpu_poll_metrics(monikor_t *mon, struct timeval *clock) {
  float values[NB_CPU_METRICS];
  int n;

  n = cpu_fetch_metrics(values, NB_CPU_METRICS);
  for (int i = 0; i < n; i++) {
    monikor_metric_push(mon, monikor_metric_float(
      metric_names[i], clock, values[i], MONIKOR_METRIC_TIMEDELTA
    ));
  }
  if (!cpu_fetch_loadavg(mon, clock))
    n++;
  return n;
}
