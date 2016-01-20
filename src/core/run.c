#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "monikor.h"

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
    if (mon->flags & MONIKOR_FLAG_RELOAD) {
      if (monikor_reload(mon))
        return 1;
    }
    mon->last_clock = next_update;
    next_update.tv_sec += mon->config->poll_interval;
    monikor_poll_modules(mon);
    monikor_send_all(mon);
    dump_store_size(mon->metrics);
    monikor_evict_metrics(mon);
    dump_store_size(mon->metrics);
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
