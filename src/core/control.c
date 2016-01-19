#include <stdlib.h>

#include "monikor.h"

void monikor_reload(monikor_t *mon) {
  (void)mon;
  monikor_log(LOG_DEBUG, "Reloading configuration\n");
}

void monikor_flush_all(monikor_t *mon) {
  monikor_log(LOG_INFO, "Flushing all caches\n");
  monikor_metric_store_flush_all(mon->metrics);
}

void monikor_send_all(monikor_t *mon) {
  monikor_send_all_metrics(mon);
}
