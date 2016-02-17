#ifndef varnish_H_
#define varnish_H_

#include <varnish/varnishapi.h>

#include "monikor.h"

#define MOD_NAME "varnish"

typedef struct {
  monikor_t *mon;
  struct timeval *clock;
  int count;
} varnish_iterator_data_t;

int varnish_poll_metrics(monikor_t *mon, struct timeval *clock, struct VSM_data *vd);

#endif /* end of include guard: varnish_H_ */
