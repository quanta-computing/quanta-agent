#ifndef process_H_
#define process_H_

#include "monikor.h"

#define MOD_NAME "process"

#define PROC_STATUSES "RSDZTW"
#define PROC_TOTAL    6
#define NB_PROC_METRICS 7

int poll_process_metrics(monikor_t *mon, struct timeval *clock);

#endif /* end of include guard: process_H_ */
