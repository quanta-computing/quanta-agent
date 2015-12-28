#ifndef CPU_H_
#define CPU_H_

#include "monikor.h"

#define MOD_NAME "cpu"

#define MNK_CPU_USEC_BETWEEN_MEASURES 1000000

int     cpu_poll_metrics(monikor_metric_list_t *metrics, time_t clock);

#endif /* end of include guard: CPU_H_ */
