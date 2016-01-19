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
  return err;
}

/*
** This is the main loop, at each run we poll the modules, send the metrics (keep them in cache if
** send failed) and perform a select(2) until we reached the next update.
*/
int monikor_run(monikor_t *mon) {
  struct timeval interval;
  struct timeval now;
  struct timeval next_update;

  gettimeofday(&next_update, NULL);
  while (42) {
    mon->last_clock = next_update;
    next_update.tv_sec += mon->config->poll_interval;
    poll_modules(mon);
    dump_metric_list(mon->metrics->current);
    monikor_send_all(mon);
    for (gettimeofday(&now, NULL); now.tv_sec < next_update.tv_sec; gettimeofday(&now, NULL)) {
      interval.tv_sec = next_update.tv_sec - now.tv_sec;
      interval.tv_usec = interval.tv_sec ? 0 : 1000;
      monikor_log(LOG_DEBUG, "Sleeping for %d seconds\n", interval.tv_sec);
      if (monikor_io_handler_poll(&mon->io_handlers, &interval) == -1)
        monikor_log(LOG_ERR, "Error in select(2): %s\n", strerror(errno));
    }
  }
  return 0;
}
