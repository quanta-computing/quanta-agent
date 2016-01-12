#include <stdlib.h>
#include <stdint.h>

#include "monikor.h"
#include "metric.h"

static size_t serialized_list_size(const monikor_metric_list_t *metrics) {
  size_t size = 0;

  for (monikor_metric_list_node_t *node = metrics->first; node; node = node->next)
    size += monikor_metric_data_size(node->metric) + strlen(node->metric->name)
      + SERIALIZED_METRIC_HDR_SIZE;
  return size + SERIALIZED_METRIC_LIST_HDR_SIZE;
}

static void serialize_metric_name_and_data(uint8_t *data, const monikor_metric_t *metric) {
  strncpy((char *)data, metric->name, strlen(metric->name));
  data += strlen(metric->name);
  switch (metric->type) {
  case MONIKOR_INTEGER:
    *((uint64_t *)data) = htonll(metric->value._int);
    break;
  case MONIKOR_FLOAT:
    *((uint32_t *)data) = htonf(metric->value._float);
    break;
  case MONIKOR_STRING:
    strncpy((char *)data, metric->value._string, strlen(metric->value._string));
    break;
  }
}

static size_t serialize_metric(uint8_t *data, const monikor_metric_t *metric) {
  *((uint64_t *)data) = htonll((uint64_t)metric->clock.tv_sec);
  *((uint64_t *)(data + sizeof(uint64_t))) = htonll((uint64_t)metric->clock.tv_usec);
  *((uint16_t *)(data + 2 * sizeof(uint64_t))) = htons((uint16_t)strlen(metric->name));
  *((uint16_t *)(data + 2 * sizeof(uint64_t) + sizeof(uint16_t))) = htons(
    (uint16_t)monikor_metric_data_size(metric));
  data[2 * sizeof(uint64_t) + 2 * sizeof(uint16_t)] = (uint8_t)metric->type;
  data[2 * sizeof(uint64_t) + 2 * sizeof(uint16_t) + sizeof(uint8_t)] = metric->flags;
  serialize_metric_name_and_data(data + SERIALIZED_METRIC_HDR_SIZE, metric);
  return monikor_metric_data_size(metric) + strlen(metric->name) + SERIALIZED_METRIC_HDR_SIZE;
}

static size_t serialize_metric_list_header(uint8_t *data, const monikor_metric_list_t *metrics,
size_t total_length) {
  *((uint16_t *)data) = htons(metrics->size);
  *((uint16_t *)(data + sizeof(uint16_t))) = htons(total_length - SERIALIZED_METRIC_LIST_HDR_SIZE);
  return SERIALIZED_METRIC_LIST_HDR_SIZE;
}

int monikor_metric_list_serialize(const monikor_metric_list_t *metrics, void **data, size_t *size) {
  uint8_t *serialized;
  size_t serialized_length;

  serialized_length = serialized_list_size(metrics);
  if (!(serialized = malloc(serialized_length)))
    return 1;
  *data = (void *)serialized;
  *size = serialized_length;
  serialized += serialize_metric_list_header(serialized, metrics, serialized_length);
  for (monikor_metric_list_node_t *node = metrics->first; node; node = node->next)
    serialized += serialize_metric(serialized, node->metric);
  return 0;
}
