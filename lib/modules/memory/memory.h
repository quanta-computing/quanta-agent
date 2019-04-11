#ifndef MEMORY_H_
#define MEMORY_H_

#include "monikor.h"
#define MOD_NAME "memory"

typedef struct {
  char *name;
  char *field_name;
  uint8_t flags;
} monikor_memory_metric_t;

int memory_poll_metrics(monikor_t *mon, struct timeval *clock);

#endif /* end of include guard: MEMORY_H_ */
