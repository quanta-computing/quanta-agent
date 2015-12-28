#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include "monikor.h"
#include "process.h"


static unsigned long compute_cpu_usage(unsigned long diff, struct timeval *interval) {
  float elapsed;

  elapsed = (float)interval->tv_usec * sysconf(_SC_CLK_TCK) / 1000000.0;
  elapsed += interval->tv_sec * sysconf(_SC_CLK_TCK);
  return 100 * (unsigned long)(1000.0 * (float)diff / elapsed);
}

static process_t *aggregate_process_metrics(process_t *process, process_metrics_t *metrics) {
  process_t *p;

  for (p = process; p && !strcmp(p->name, process->name); p = p->next) {
    if (p->dirty)
      continue;
    metrics->count++;
    metrics->cpu_user_usage += compute_cpu_usage(
      p->metrics.cpu_user_usage, &p->metrics.interval);
    metrics->cpu_kernel_usage += compute_cpu_usage(
      p->metrics.cpu_kernel_usage, &p->metrics.interval);
    metrics->mem_vsize += p->metrics.mem_vsize;
    metrics->mem_rss += p->metrics.mem_rss;
  }
  return p;
}

static void push_process_metrics(monikor_metric_list_t *metrics, time_t clock,
process_t *process, process_metrics_t *proc_metrics) {
  char metric_name[512];
  char *metric_base_end;

  metric_base_end = metric_name + sprintf(metric_name, "process.%s.", process->name);
  strcpy(metric_base_end, "cpu.user");
  monikor_metric_list_push(metrics, monikor_metric_float(
    metric_name, clock, (float)proc_metrics->cpu_user_usage / 1000.0
  ));
  strcpy(metric_base_end, "cpu.kernel");
  monikor_metric_list_push(metrics, monikor_metric_float(
    metric_name, clock, (float)proc_metrics->cpu_kernel_usage / 1000.0
  ));
  strcpy(metric_base_end, "mem.vsize");
  monikor_metric_list_push(metrics, monikor_metric_integer(
    metric_name, clock, proc_metrics->mem_vsize
  ));
  strcpy(metric_base_end, "mem.rss");
  monikor_metric_list_push(metrics, monikor_metric_integer(
    metric_name, clock, proc_metrics->mem_rss
  ));
  strcpy(metric_base_end, "count");
  monikor_metric_list_push(metrics, monikor_metric_integer(
    metric_name, clock, proc_metrics->count
  ));
}

void aggregate_metrics(monikor_metric_list_t *metrics, process_list_t *list, time_t clock) {
  process_metrics_t proc_metrics;
  process_t *next;
  process_t *p;

  p = list->first;
  while (p) {
    if (p->dirty) {
      p = p->next;
      continue;
    }
    process_metrics_init(&proc_metrics);
    next = aggregate_process_metrics(p, &proc_metrics);
    push_process_metrics(metrics, clock, p, &proc_metrics);
    p = next;
  }
}
