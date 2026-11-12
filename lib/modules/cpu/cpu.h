#ifndef CPU_H_
#define CPU_H_

#include "monikor.h"

#define MOD_NAME "cpu"

#define MNK_CPU_USEC_BETWEEN_MEASURES 1000000

#define MNK_CPU_NB_METRICS 9

// Previous poll: raw /proc/stat jiffies and clock
typedef struct {
  uint64_t        jiffies[MNK_CPU_NB_METRICS];
  size_t          n_jiffies;
  struct timeval  clock;
} monikor_cpu_mod_t;

int     cpu_poll_metrics(monikor_t *mon, struct timeval *clock, monikor_cpu_mod_t *mod);

#endif /* end of include guard: CPU_H_ */
