#include <sys/time.h>

#include "monikor.h"

void monikor_update(monikor_t *mon, struct timeval *clock) {
  if (mon->last_clock.tv_sec + mon->config->update_interval > clock->tv_sec)
    return;
  mon->last_clock.tv_sec += mon->config->update_interval;
  monikor_send_all(mon);
  dump_store_size(mon->metrics);
  monikor_evict_metrics(mon);
  dump_store_size(mon->metrics);
}
