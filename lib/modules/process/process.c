#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "monikor.h"
#include "process.h"

void process_init(void) {
}

void process_cleanup(void) {
}


monikor_metric_list_t *process_poll(void) {
  monikor_metric_list_t *metrics;
  process_list_t *processes;
  time_t now = time(NULL);

  if (!(metrics = monikor_metric_list_new()))
    return NULL;
  if (!(processes = fetch_processes())) {
    monikor_metric_list_free(metrics);
    return NULL;
  }
  poll_all_processes(processes);
  usleep(MNK_PROCESS_USEC_BETWEEN_MEASURES);
  poll_all_processes(processes);
  aggregate_metrics(metrics, processes, now);
  process_list_free(processes);
  return metrics;
}
