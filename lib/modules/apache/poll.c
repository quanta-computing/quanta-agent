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

static char *get_apache_status(void) {
  char http_query[1024];
  size_t http_query_size;

  http_query_size = snprintf(http_query, sizeof(http_query),
    "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", APACHE_SERVER_URL, APACHE_HOST
  );
  if (http_query_size >= sizeof(http_query))
    return NULL;
  return monikor_net_csr(APACHE_HOST, APACHE_PORT, http_query);

}

static int fetch_metric(const char *apache_status, const char *name, unsigned *value) {
  char *line;

  if (!(line = strstr(apache_status, name)))
    return 1;
  *value = strtol(line + strlen(name), NULL, 10);
  return 0;
}

int apache_poll_metrics(monikor_metric_list_t *metrics, struct timeval *clock) {
  int n = 0;
  char *status;
  unsigned value;

  if (!(status = get_apache_status()))
    return 0;
  for (size_t i = 0; apache_metrics[i].name; i++) {
    if (!fetch_metric(status, apache_metrics[i].field_name, &value)) {
      if (!strcmp(apache_metrics[i].name, "apache.workers.bytes_per_second"))
        value *= 1024;
      monikor_metric_list_push(metrics, monikor_metric_integer(
        apache_metrics[i].name, clock, value, apache_metrics[i].flags
      ));
      n++;
    }
  }
  free(status);
  return n;
}
