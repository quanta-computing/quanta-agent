#include <stdlib.h>

#include "monikor.h"

void monikor_evict_metrics(monikor_t *mon) {
  struct timeval clock;
  size_t n;

  gettimeofday(&clock, NULL);
  clock.tv_sec -= 3 * mon->config->poll_interval;
  n = monikor_metric_store_evict_delta(mon->metrics, &clock)
    + monikor_metric_store_evict_cache(mon->metrics, mon->config->max_cache_size);
  monikor_log(LOG_INFO, "Evicted %zu metrics from caches\n", n);
}
