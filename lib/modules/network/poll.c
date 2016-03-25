#include <stdlib.h>

#include "monikor.h"
#include "network.h"


static const char *net_metrics[] = {
  "rx.bytes", "rx.packets", "rx.errors", "rx.dropped", NULL, NULL, NULL, NULL,
  "tx.bytes", "tx.packets", "tx.errors", "tx.dropped", NULL, NULL, NULL, NULL
};
#define NB_METRICS (sizeof(net_metrics) / sizeof(*net_metrics))

static int poll_device_metrics(monikor_t *mon, struct timeval *clock, char *name, char *info) {
  int n = 0;
  char *metric;
  char *metric_base_end;
  char metric_name[256];
  uint64_t value;

  metric_base_end = metric_name + sprintf(metric_name, "network.interfaces.%s.", name);
  metric = info;
  for (size_t i = 0; i < NB_METRICS; i++) {
    value = strtoull(metric, &metric, 10);
    if (net_metrics[i]) {
      strcpy(metric_base_end, net_metrics[i]);
      monikor_metric_push(mon, monikor_metric_integer(
        metric_name, clock, value, MONIKOR_METRIC_TIMEDELTA
      ));
      n++;
    }
  }
  return n;
}

/* Set the pointers to devname and devinfo from netinfo
** Netinfo is modified to put \0 in appropriate places so devname and devinfo are null terminated
** This function returns a pointer to the byte after devinfo.
** This function returns NULL if no device was found
*/
static char *next_device_info(char *netinfo, char **devname, char **devinfo) {
  char *end;

  if (!(end = strchr(netinfo, ':')))
    return NULL;
  for (*devname = end; **devname != ' ' && *devname != netinfo; *devname -= 1);
  *devname += 1;
  *devinfo = end + 1;
  *end = 0;
  if (!(end = strchr(*devinfo, '\n')))
    return NULL;
  *end = 0;
  return end + 1;
}

static inline int is_valid_device(const char *name) {
  return strlen(name) <= MNK_NET_MAX_DEV_LEN && strncmp(name, "lo", 2);
}


int poll_network_metrics(monikor_t *mon, struct timeval *clock) {
  int n = 0;
  char *netinfo;
  char *rest;
  char *dev;
  char *info;

  if (!(netinfo = monikor_read_file("/proc/net/dev")))
    return 0;
  rest = netinfo;
  while ((rest = next_device_info(rest, &dev, &info))) {
    if (is_valid_device(dev))
      n += poll_device_metrics(mon, clock, dev, info);
  }
  free(netinfo);
  return n;
}
