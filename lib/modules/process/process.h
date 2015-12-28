#ifndef PROCESS_H_
#define PROCESS_H_

#include <stdint.h>
#include <sys/time.h>

#include "monikor.h"

#define MOD_NAME "process"
#define MAX_PROCESS_PROCFILE_LEN 1024
#define MNK_PROCESS_USEC_BETWEEN_MEASURES 1000000

typedef struct {
  unsigned count;
  struct timeval interval;
  unsigned long cpu_user_usage;
  unsigned long cpu_kernel_usage;
  unsigned long mem_vsize;
  unsigned long mem_rss;
} process_metrics_t;

typedef struct process {
  struct process *next;
  char *name;
  pid_t pid;
  process_metrics_t metrics;
  uint8_t dirty;
} process_t;

typedef struct {
  process_t *first;
  process_t *last;
  size_t size;
} process_list_t;


void process_metrics_init(process_metrics_t *metrics);
process_t *process_new(const char *name, pid_t pid);
void process_free(process_t *p);
void process_dump(process_t *p);

process_list_t *process_list_new(void);
void process_list_free(process_list_t *list);
void process_list_push(process_list_t *list, process_t *process);

process_list_t *fetch_processes(void);
int fetch_process_metrics(process_t *process);
void poll_all_processes(process_list_t *list);
void aggregate_metrics(monikor_metric_list_t *metrics, process_list_t *list, time_t clock);

#endif /* end of include guard: PROCESS_H_ */
