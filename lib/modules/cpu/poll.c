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

unsigned cpu_fetch_metrics(float *values, size_t n_metrics) {
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
    values[i] = strtof(start, NULL);
    start = end + 1;
  }
  free(proc_stat);
  return i;
}

int cpu_poll_metrics(monikor_metric_list_t *metrics, time_t clock) {
  float values_0[NB_CPU_METRICS];
  float values_1[NB_CPU_METRICS];
  unsigned n;
  float timedelta = 0.0;
  // float sum = 0.0;
  // float value;

  n = cpu_fetch_metrics(values_0, NB_CPU_METRICS);
  usleep(MNK_CPU_USEC_BETWEEN_MEASURES);
  if (cpu_fetch_metrics(values_1, NB_CPU_METRICS) != n)
    return 0;
  for (size_t i = 0; i < n; i++)
    timedelta += values_1[i] - values_0[i];
  for (size_t i = 0; i < n; i++) {
    // value = 100.0 * ((values_1[i] - values_0[i]) / timedelta);
    // sum += value;
    monikor_metric_list_push(metrics, monikor_metric_float(metric_names[i], clock,
      100.0 * ((values_1[i] - values_0[i]) / timedelta)
    ));
  }
  return n;
}
