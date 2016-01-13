#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

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
  hdr.clock_sec = ntohll(*((uint64_t *)(data + METRIC_HDR_CLOCK_SEC_OFF)));
  hdr.clock_usec = ntohll(*((uint64_t *)(data + METRIC_HDR_CLOCK_USEC_OFF)));
  hdr.name_size = ntohs(*((uint16_t *)(data + METRIC_HDR_NAME_SIZE_OFF)));
  hdr.data_size = ntohs(*((uint16_t *)(data + METRIC_HDR_DATA_SIZE_OFF)));
  hdr.type = data[METRIC_HDR_TYPE_OFF];
  hdr.flags = data[METRIC_HDR_FLAGS_OFF];
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

void monikor_metric_list_header_unserialize(void *data, monikor_serialized_metric_list_hdr_t *hdr) {
  uint8_t *serialized = data;

  hdr->version = serialized[METRIC_LIST_HDR_VERSION_OFF];
  hdr->count = ntohs(*((uint16_t *)(serialized + METRIC_LIST_HDR_COUNT_OFF)));
  hdr->data_size = ntohs(*((uint16_t *)(serialized + METRIC_LIST_HDR_DATA_SIZE_OFF)));
}

size_t monikor_metric_list_unserialize(void *data, monikor_serialized_metric_list_hdr_t *hdr,
monikor_metric_list_t **metrics) {
  size_t i;
  monikor_metric_t *metric;
  uint8_t *serialized = (uint8_t *)data;

  if (!(*metrics = monikor_metric_list_new()))
    return 0;
  for (i = 0; i < hdr->count && serialized < (uint8_t *)data + hdr->data_size; i++) {
    if (!(metric = metric_from_serialized(serialized)))
      return i;
    monikor_metric_list_push(*metrics, metric);
    serialized += SERIALIZED_METRIC_HDR_SIZE + strlen(metric->name)
      + monikor_metric_data_size(metric);
  }
  return i;
}
