#include <time.h>

#include "monikor.h"
#include "cpu.h"

void cpu_init(void) {
}

void cpu_cleanup(void) {
}

monikor_metric_list_t *cpu_poll(void) {
  monikor_metric_list_t *metrics;
  time_t now = time(NULL);

  if (!(metrics = monikor_metric_list_new()))
    goto err;
  if (!cpu_poll_metrics(metrics, now))
    goto err;
  return metrics;

err:
  monikor_log_mod(LOG_ERR, MOD_NAME, "Cannot poll CPU metrics\n");
  monikor_metric_list_free(metrics);
  return NULL;
}
