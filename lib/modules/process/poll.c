#include <unistd.h>
#include <sys/time.h>

#include "monikor.h"
#include "process.h"

static void parse_process_metrics(char *proc_info, process_metrics_t *metrics) {
  unsigned idx = 0;
  char *value;
  char *save;

  for (value = strtok_r(proc_info, " ", &save); value; value = strtok_r(NULL, " ", &save)) {
    switch (idx) {
    case 13:
      metrics->cpu_user_usage = strtol(value, NULL, 10);
      break;
    case 14:
      metrics->cpu_kernel_usage = strtol(value, NULL, 10);
      break;
    case 22:
      metrics->mem_vsize = strtol(value, NULL, 10);
      break;
    case 23:
      metrics->mem_rss = strtol(value, NULL, 10) * getpagesize();
      break;
    }
    idx++;
  }
}

static int read_process_metrics(pid_t pid, process_metrics_t *metrics) {
  char filepath[256];
  char *proc_info;

  sprintf(filepath, "/proc/%d/stat", pid);
  gettimeofday(&metrics->interval, NULL);
  if (!(proc_info = monikor_read_file(filepath))) {
    monikor_log_mod(LOG_WARNING, MOD_NAME, "Cannot read file %s\n", filepath);
    return 1;
  }
  parse_process_metrics(proc_info, metrics);
  free(proc_info);
  return 0;
}

static void update_process_metrics(process_metrics_t *first, process_metrics_t *second) {
  first->interval.tv_sec = second->interval.tv_sec - first->interval.tv_sec;
  first->interval.tv_usec = second->interval.tv_usec - first->interval.tv_usec;
  first->cpu_user_usage = second->cpu_user_usage - first->cpu_user_usage;
  first->cpu_kernel_usage = second->cpu_kernel_usage - first->cpu_kernel_usage;
}

/*
** On firt pass, we just fetch the metrics, on the second time we substract the already fetched
** metrics to the new ones to keep only the difference
*/
int fetch_process_metrics(process_t *process) {
  process_metrics_t metrics;

  if (!process->metrics.interval.tv_sec)
    return read_process_metrics(process->pid, &process->metrics);
  else if (read_process_metrics(process->pid, &metrics))
    return 1;
  update_process_metrics(&process->metrics, &metrics);
  return 0;
}

void poll_all_processes(process_list_t *list) {
  for (process_t *p = list->first; p; p = p->next) {
    if (p->dirty)
      continue;
    if (fetch_process_metrics(p))
      p->dirty = 1;
  }
}
