#include <stdlib.h>
#include <string.h>

#include "monikor.h"
#include "network.h"

void device_metrics_init(device_metrics_t *metrics) {
  metrics->bytes = 0;
  metrics->packets = 0;
  metrics->errors = 0;
  metrics->dropped = 0;
}

net_dev_t *net_dev_new(const char *name) {
  net_dev_t *dev;

  if (!(dev = malloc(sizeof(*dev)))
  || !(dev->name = strdup(name))) {
    free(dev);
    return NULL;
  }
  dev->next = NULL;
  dev->interval.tv_sec = 0;
  dev->interval.tv_usec = 0;
  device_metrics_init(&dev->rx);
  device_metrics_init(&dev->tx);
  return dev;
}

void net_dev_free(net_dev_t *dev) {
  if (!dev)
    return;
  free(dev->name);
  free(dev);
}

net_dev_list_t *net_dev_list_new(void) {
  net_dev_list_t *l;

  if (!(l = malloc(sizeof(*l))))
    return NULL;
  l->first = NULL;
  l->last = NULL;
  l->size = 0;
}

void net_dev_list_free(net_dev_list_t *list) {
  net_dev_t *next;

  if (!list)
    return;
  for (net_dev_t *dev = list->first; dev; dev = next) {
    next = dev->next;
    net_dev_free(dev);
  }
}

void net_dev_list_push(net_dev_list_t *list, net_dev_t *dev) {
  if (!list->size)
    list->first = dev;
  else
    list->last->next = dev;
  list->last = dev;
  list->size++;
}

net_dev_t *net_dev_list_find(net_dev_list_t *list, const char *name) {
  for (net_dev_t *dev = list->first; dev; dev = dev->next)
    if (!strcmp(dev->name, name))
      return dev;
  return NULL;
}
