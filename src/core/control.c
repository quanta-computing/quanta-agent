#include <stdlib.h>

#include "monikor.h"

void monikor_flag_reload(monikor_t *mon) {
  mon->flags |= MONIKOR_FLAG_RELOAD;
}

void monikor_flush_all(monikor_t *mon) {
  monikor_log(LOG_INFO, "Flushing all caches\n");
  monikor_metric_store_flush_all(mon->metrics);
}

void monikor_send_all(monikor_t *mon) {
  monikor_send_all_metrics(mon);
}
