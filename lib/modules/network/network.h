#ifndef network_H_
#define network_H_

#include "monikor.h"

#define MOD_NAME "network"
#define MNK_NET_MAX_DEV_LEN 10

int poll_network_metrics(monikor_metric_list_t *metrics, struct timeval *clock);

#endif /* end of include guard: network_H_ */
