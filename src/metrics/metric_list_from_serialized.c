#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "monikor.h"
#include "metric.h"


static int metric_data_from_serialized(uint8_t *data, size_t data_size, monikor_metric_t *metric) {
  switch (metric->type) {
  case MONIKOR_INTEGER:
    metric->value._int = ntohll(*((uint64_t *)data));
    break;
  case MONIKOR_FLOAT:
    metric->value._float = ntohf(*((uint32_t *)data));
    break;
  case MONIKOR_STRING:
    if (!(metric->value._string = strndup((char *)data, data_size)))
      return 1;
    break;
  default:
    return 1;
  }
  return 0;
}

static monikor_metric_t *metric_from_serialized(uint8_t *data) {
  monikor_metric_t *metric;
  monikor_serialized_metric_hdr_t hdr;

  if (!(metric = malloc(sizeof(*metric))))
    return NULL;
  hdr.clock_sec = ntohll(*((uint64_t *)data));
  hdr.clock_usec = ntohll(*((uint64_t *)(data + sizeof(uint64_t))));
  hdr.name_size = ntohs(*((uint16_t *)(data + 2 * sizeof(uint64_t))));
  hdr.data_size = ntohs(*((uint16_t *)(data + 2 * sizeof(uint64_t) + sizeof(uint16_t))));
  hdr.type = data[2 * sizeof(uint64_t) + 2 * sizeof(uint16_t)];
  hdr.flags = data[2 * sizeof(uint64_t) + 2 * sizeof(uint16_t) + sizeof(uint8_t)];
  metric->clock.tv_sec = (time_t)hdr.clock_sec;
  metric->clock.tv_usec = (suseconds_t)hdr.clock_usec;
  metric->flags = hdr.flags;
  metric->type = hdr.type;
  if (!(metric->name = strndup((char *)(data + SERIALIZED_METRIC_HDR_SIZE), hdr.name_size))) {
    free(metric);
    return NULL;
  }
  if (metric_data_from_serialized(data + SERIALIZED_METRIC_HDR_SIZE + hdr.name_size,
  hdr.data_size, metric)) {
    free(metric->name);
    free(metric);
    return NULL;
  }
  return metric;
}

size_t monikor_metric_list_from_serialized(void *data, size_t size,
monikor_metric_list_t **metrics) {
  size_t i;
  uint16_t n_metrics;
  monikor_metric_t *metric;
  uint8_t *serialized = (uint8_t *)data;

  if (!(*metrics = monikor_metric_list_new()))
    return 0;
  n_metrics = ntohs(*((uint16_t *)serialized));
  serialized += SERIALIZED_METRIC_LIST_HDR_SIZE;
  for (i = 0; i < n_metrics && serialized < (uint8_t *)data + size; i++) {
    if (!(metric = metric_from_serialized(serialized)))
      return i;
    monikor_metric_list_push(*metrics, metric);
    serialized += SERIALIZED_METRIC_HDR_SIZE + strlen(metric->name)
      + monikor_metric_data_size(metric);
  }
  return i;
}
