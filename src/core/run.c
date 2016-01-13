#include <stdio.h>
#include <errno.h>
#include <string.h>
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
  monikor_metric_store_flush(mon->metrics);
  return err;
}

int monikor_run(monikor_t *mon) {
  struct timeval interval;
  struct timeval now;
  struct timeval next_update;

  while (42) {
    gettimeofday(&mon->last_clock, NULL);
    next_update = mon->last_clock;
    next_update.tv_sec += mon->config->poll_interval;
    poll_modules(mon);
    for (gettimeofday(&now, NULL); now.tv_sec < next_update.tv_sec; gettimeofday(&now, NULL)) {
      interval.tv_sec = next_update.tv_sec - now.tv_sec;
      interval.tv_usec = 0;
      monikor_log(LOG_DEBUG, "Sleeping for %d\n", interval.tv_sec);
      if (monikor_io_handler_poll(&mon->io_handlers, &interval) == -1)
        monikor_log(LOG_ERR, "Error in select(2): %s\n", strerror(errno));
    }
  }
  return 0;
}
