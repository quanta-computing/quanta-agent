#include <stdlib.h>

#include "monikor.h"
#include "apache.h"

static const struct {
  char *name;
  char *field_name;
  uint8_t flags;
} apache_metrics[] = {
  {"apache.workers.busy", "BusyWorkers:", 0},
  {"apache.workers.idle", "IdleWorkers:", 0},
  {"apache.workers.requests_per_second", "Total Accesses:", MONIKOR_METRIC_TIMEDELTA},
  {"apache.workers.bytes_per_second", "Total kBytes:", MONIKOR_METRIC_TIMEDELTA},
  {NULL, NULL, 0}
};

static int fetch_metric(const char *apache_status, const char *name, uint64_t *value) {
  char *line;

  if (!(line = strstr(apache_status, name)))
    return 1;
  *value = strtoull(line + strlen(name), NULL, 10);
  return 0;
}

int apache_poll_metrics(monikor_t *mon, struct timeval *clock, const char *url) {
  int n = 0;
  http_response_t *status;
  uint64_t value;
  uint64_t total_workers = 0;

  if (!(status = monikor_http_get(url, APACHE_TIMEOUT)))
    return -1;
  if (status->code != 200) {
    free(status->data);
    free(status);
    return -1;
  }
  for (size_t i = 0; apache_metrics[i].name; i++) {
    if (!fetch_metric(status->data, apache_metrics[i].field_name, &value)) {
      if (!strcmp(apache_metrics[i].name, "apache.workers.bytes_per_second"))
        value *= 1024;
      if (!strcmp(apache_metrics[i].name, "apache.workers.busy")
      || !strcmp(apache_metrics[i].name, "apache.workers.idle"))
        total_workers += value;
      monikor_metric_push(mon, monikor_metric_integer(
        apache_metrics[i].name, clock, value, apache_metrics[i].flags
      ));
      n++;
    }
  }
  monikor_metric_push(mon, monikor_metric_integer("apache.workers.total", clock, total_workers, 0));
  free(status->data);
  free(status);
  return n + 1;
}
