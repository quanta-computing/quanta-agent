#ifndef VARNISH_TEST_STUBS_H_
#define VARNISH_TEST_STUBS_H_

#include <stddef.h>
#include <stdint.h>

// Stand-ins for the agent core: metrics are recorded, logs go to stderr only with VARNISH_TEST_VERBOSE

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
