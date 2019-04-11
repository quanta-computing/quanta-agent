#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "monikor.h"

static int get_tick_interval(monikor_t *mon) {
  int tick = mon->config->update_interval < mon->config->poll_interval ?
    mon->config->update_interval : mon->config->poll_interval;

  for (size_t i = 0; i < mon->modules.count; i++) {
    if (mon->modules.modules[i]->poll_interval < tick)
      tick = mon->modules.modules[i]->poll_interval;
  }
  return tick;
}

/*
** This is the main loop, at each run we poll the modules, send the metrics (keep them in cache if
** send failed) and perform a select(2) until we reached the next update.
*/
int monikor_run(monikor_t *mon) {
  struct timeval interval;
  struct timeval now;
  time_t next_update;
  int tick;

  next_update = time(NULL);
  tick = get_tick_interval(mon);
  while (42) {
    if (mon->flags & MONIKOR_FLAG_RELOAD) {
      if (monikor_reload(mon))
        return 1;
      tick = get_tick_interval(mon);
    }
    gettimeofday(&now, NULL);
    next_update += tick;
    // next_update + tick might be greater than now.tv_sec when the process was stopped or traced
    if (next_update < now.tv_sec) {
      monikor_log(LOG_WARNING, "We shifted too much, rebasing next_update to now\n");
      next_update = now.tv_sec;
    }
    monikor_poll_modules(mon, &now);
    if (monikor_logger_level() <= LOG_DEBUG)
      dump_metric_list(mon->metrics->current);
    monikor_update(mon, &now);
    do {
      gettimeofday(&now, NULL);
      // We cannot have negative timeouts so we put at least 10us
      interval.tv_sec = next_update > now.tv_sec ? next_update - now.tv_sec : 0;
      interval.tv_usec = interval.tv_sec ? 0 : 10;
      if (monikor_io_handler_poll(&mon->io_handlers, &interval) == -1)
        monikor_log(LOG_ERR, "Error in select(2): %s\n", strerror(errno));

    } while (now.tv_sec < next_update);
  }
  return 0;
}
