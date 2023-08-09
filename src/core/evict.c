#include <stdlib.h>

#include "monikor.h"

void monikor_evict_metrics(monikor_t *mon) {
  struct timeval max_delta_clock;
  size_t n;

  gettimeofday(&max_delta_clock, NULL);
  max_delta_clock.tv_sec -= 3 * mon->config->poll_interval;
  n = monikor_metric_store_evict_delta(mon->metrics, &max_delta_clock)
    + monikor_metric_store_evict_cache(mon->metrics, mon->config->cache.max_size);
  monikor_log(LOG_INFO, "Evicted %zu metrics from caches\n", n);
}
