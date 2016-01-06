#ifndef network_H_
#define network_H_

#include "monikor.h"

#define MOD_NAME "network"
#define MNK_NET_USEC_BETWEEN_MEASURES 1000000
#define MNK_NET_MAX_DEV_LEN 10

typedef struct {
  unsigned long bytes;
  unsigned long packets;
  unsigned long errors;
  unsigned long dropped;
} device_metrics_t;

typedef struct net_dev {
  struct net_dev *next;
  char *name;
  struct timeval interval;
  device_metrics_t rx;
  device_metrics_t tx;
} net_dev_t;

typedef struct {
  net_dev_t *first;
  net_dev_t *last;
  size_t size;
} net_dev_list_t;


void device_metrics_init(device_metrics_t *metrics);
net_dev_t *net_dev_new(const char *name);
void net_dev_free(net_dev_t *dev);

net_dev_list_t *net_dev_list_new(void);
void net_dev_list_free(net_dev_list_t *list);
void net_dev_list_push(net_dev_list_t *list, net_dev_t *dev);
net_dev_t *net_dev_list_find(net_dev_list_t *list, const char *name);

int poll_network_metrics(monikor_metric_list_t *metrics, struct timeval *clock);

#endif /* end of include guard: network_H_ */
