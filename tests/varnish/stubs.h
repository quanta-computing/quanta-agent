#ifndef VARNISH_TEST_STUBS_H_
#define VARNISH_TEST_STUBS_H_

#include <stddef.h>
#include <stdint.h>

/*
** The varnish module is linked into the tests with these stand-ins for the agent core: metrics
** are recorded instead of being stored and sent, log lines go to stderr only when
** VARNISH_TEST_VERBOSE is set.
*/

#define RECORDED_METRICS_MAX 128

typedef struct {
  char name[256];
  uint64_t value;
  uint8_t flags;
} recorded_metric_t;

extern recorded_metric_t recorded_metrics[RECORDED_METRICS_MAX];
extern size_t recorded_metrics_count;

void recorded_metrics_reset(void);
const recorded_metric_t *recorded_metric_find(const char *name);

#endif /* end of include guard: VARNISH_TEST_STUBS_H_ */
