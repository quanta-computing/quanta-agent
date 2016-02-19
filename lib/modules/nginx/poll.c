#include <stdlib.h>
#include <curl/curl.h>

#include "monikor.h"
#include "nginx.h"


static int fetch_qps(monikor_t *mon, struct timeval *clock, char *status) {
  char *metric;
  char *next;

  if (!(metric = strstr(status, "server accepts handled requests"))
  || !(metric = strchr(metric, '\n')))
    return 0;
  metric++;
  strtoull(metric, &next, 10);
  metric = next;
  strtoull(metric, &next, 10);
  metric = next;
  monikor_metric_push(mon, monikor_metric_integer("nginx.requests_per_second",
    clock, (uint64_t)strtoull(metric, NULL, 10), MONIKOR_METRIC_TIMEDELTA
  ));
  return 1;
}

static int fetch_conn_metric(char *status, const char *name, uint64_t *value) {
  char *metric;

  if (!(metric = strstr(status, name))
  || !(metric = strchr(metric, ' ')))
    return 0;
  *value = (uint64_t)strtoull(metric, NULL, 10);
  return 1;
}

static int fetch_metrics(monikor_t *mon, struct timeval *clock, char *status) {
  int n = 0;
  uint64_t value;

  n += fetch_qps(mon, clock, status);
  if (fetch_conn_metric(status, "Reading", &value)) {
    monikor_metric_push(mon, monikor_metric_integer("nginx.connections.reading", clock, value, 0));
    n++;
  }
  if (fetch_conn_metric(status, "Writing", &value)) {
    monikor_metric_push(mon, monikor_metric_integer("nginx.connections.writing", clock, value, 0));
    n++;
  }
  if (fetch_conn_metric(status, "Waiting", &value)) {
    monikor_metric_push(mon, monikor_metric_integer("nginx.connections.waiting", clock, value, 0));
    n++;
  }
  return n;
}

static void nginx_fetch_metrics(http_response_t *status, CURLcode result) {
  struct timeval clock;
  int n;

  if (result == CURLE_OK && status->code == 200) {
    gettimeofday(&clock, NULL);
    n = fetch_metrics((monikor_t *)status->userdata, &clock, status->data);
    monikor_log_mod(LOG_INFO, MOD_NAME, "Got %d nginx metrics\n", n);
  }
  free(status->data);
  free(status);
}

int nginx_poll_metrics(monikor_t *mon, struct timeval *clock, const char *url) {
  (void)clock;
  if (monikor_http_get(mon, url, NGINX_TIMEOUT, &nginx_fetch_metrics, (void *)mon))
    return -1;
  return 0;
}
