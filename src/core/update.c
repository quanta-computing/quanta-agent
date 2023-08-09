#include <time.h>
#include <sys/time.h>
#include <math.h>

#include "monikor.h"

#define MONIKOR_UPDATE_BACKOFF_COUNT_MAX  15
#define MONIKOR_UPDATE_BACKOFF_EXP        2


void monikor_update(monikor_t *mon, struct timeval *clock) {
  if (mon->update.last_clock.tv_sec + mon->config->update_interval > clock->tv_sec)
    return;
  mon->update.last_clock = *clock;
  dump_store_size(mon->metrics);
  monikor_evict_metrics(mon);
  monikor_send_all(mon);
}

void monikor_update_init(monikor_t *mon) {
  gettimeofday(&mon->update.last_clock, NULL);
  mon->update.next_update = time(NULL);
}

void monikor_update_backoff(monikor_t *mon) {
  // We don't backoff on the first failure
  if (mon->update.backoff_count > 0) {
    time_t backoff_sec = mon->config->update_interval;

    backoff_sec *= (time_t)powf(
      (float)MONIKOR_UPDATE_BACKOFF_EXP,
      (float)mon->update.backoff_count
    );
    monikor_log(LOG_WARNING, "Delaying next update using exponential backoff: +%zus (count=%zu)\n", backoff_sec, mon->update.backoff_count);
    mon->update.next_update += backoff_sec;
    monikor_metric_store_flush_all(mon->metrics);
  }
  // We want to ensure we don't increment past a certain point to avoid too much
  // backoff delay and thus possible overflow issues
  if (mon->update.backoff_count < MONIKOR_UPDATE_BACKOFF_COUNT_MAX)
    mon->update.backoff_count += 1;
}

void monikor_update_success(monikor_t *mon) {
  mon->update.backoff_count = 0;
}
