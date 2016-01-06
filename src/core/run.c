#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include "monikor.h"

static int poll_modules(monikor_t *mon) {
  monikor_metric_list_t *metrics;
  int err = 0;

  for (size_t i = 0; i < mon->modules.count; i++) {
    monikor_log(LOG_INFO, "Polling module %lu %s\n", i, mon->modules.modules[i]->name);
    metrics = mon->modules.modules[i]->poll();
    if (metrics)
      monikor_metric_store_lpush(mon->metrics, metrics);
    else {
      monikor_log(LOG_WARNING, "No data for module %s\n", mon->modules.modules[i]->name);
      err = 1;
    }
    monikor_metric_list_free(metrics);
  }
  printf("CURRENT (%lu):\n", mon->metrics->current->size);
  dump_metric_list(mon->metrics->current);
  // printf("DELTA (%u):\n", mon->metrics->delta->size);
  // dump_metric_list(mon->metrics->delta);
  monikor_metric_store_flush(mon->metrics);
  return err;
}

static int perform_sleep(monikor_t *mon) {
  int sleep_amount;
  struct timeval now;

  gettimeofday(&now, NULL);
  sleep_amount = 1000000 * (mon->last_clock.tv_sec + mon->config->poll_interval - now.tv_sec);
  sleep_amount += mon->last_clock.tv_usec - now.tv_usec;
  if (sleep_amount < 0)
    return 0;
  monikor_log(LOG_DEBUG, "Sleeping for %dms\n", sleep_amount / 1000);
  usleep(sleep_amount);
  return 0;
}

int monikor_run(monikor_t *mon) {
  while (42) {
    gettimeofday(&mon->last_clock, NULL);
    poll_modules(mon);
    perform_sleep(mon);
  }
  return 0;
}
