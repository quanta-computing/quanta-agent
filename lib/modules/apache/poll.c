#include <stdlib.h>

#include "monikor.h"
#include "apache.h"

static struct {
  time_t last_clock;
  unsigned total_accesses;
  unsigned total_bytes;
} delta_metrics = {0, 0, 0};

static const struct {
  char *name;
  char *field_name;
} apache_metrics[] = {
  {"apache.workers.busy", "BusyWorkers:"},
  {"apache.workers.idle", "IdleWorkers:"},
  {NULL, NULL}
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

static int poll_delta_metrics(monikor_metric_list_t *metrics, const char *status, time_t clock) {
  int n = 0;
  unsigned value;

  if (!fetch_metric(status, "Total Accesses:", &value)) {
    if (delta_metrics.last_clock) {
      monikor_metric_list_push(metrics, monikor_metric_float("apache.queries_per_second", clock,
          (float)(value - delta_metrics.total_accesses) / (float)(clock - delta_metrics.last_clock)
      ));
      n++;
    }
    delta_metrics.total_accesses = value;
  }
  if (!fetch_metric(status, "Total kBytes:", &value)) {
    value *= 1024;
    if (delta_metrics.last_clock) {
      monikor_metric_list_push(metrics, monikor_metric_float("apache.bytes_per_second", clock,
          (float)(value - delta_metrics.total_bytes) / (float)(clock - delta_metrics.last_clock)
      ));
      n++;
    }
    delta_metrics.total_bytes = value;
  }
  delta_metrics.last_clock = clock;
  return n;
}

int apache_poll_metrics(monikor_metric_list_t *metrics, time_t clock) {
  int n = 0;
  char *status;
  unsigned value;

  if (!(status = get_apache_status()))
    return 0;
  for (size_t i = 0; apache_metrics[i].name; i++) {
    if (!fetch_metric(status, apache_metrics[i].field_name, &value)) {
      monikor_metric_list_push(metrics, monikor_metric_integer(
        apache_metrics[i].name, clock, value
      ));
      n++;
    }
  }
  n += poll_delta_metrics(metrics, status, clock);
  free(status);
  return n;
}
