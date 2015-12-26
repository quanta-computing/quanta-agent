#include <unistd.h>
#include <sys/time.h>

#include "monikor.h"

static int poll_modules(monikor_t *mon) {
  monikor_metric_list_t *metrics;
  int err = 0;

  for (size_t i = 0; i < mon->modules.count; i++) {
    monikor_log(LOG_INFO, "Polling module %d %s\n", i, mon->modules.modules[i]->name);
    metrics = mon->modules.modules[i]->poll();
    if (metrics)
      dump_metric_list(metrics);
    else {
      monikor_log(LOG_INFO, "No data for module %s", mon->modules.modules[i]->name);
      err = 1;
    }
    monikor_metric_list_free(metrics);
  }
  return err;
}

static int perform_sleep(monikor_t *mon) {
  useconds_t sleep_amount;
  struct timeval now;

  gettimeofday(&now, NULL);
  sleep_amount = 1000000 * (mon->last_clock.tv_sec + mon->config->poll_interval - now.tv_sec);
  sleep_amount += mon->last_clock.tv_usec - now.tv_usec;
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
