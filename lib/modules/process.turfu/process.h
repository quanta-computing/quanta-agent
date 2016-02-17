#ifndef PROCESS_H_
#define PROCESS_H_

#include <stdint.h>
#include <sys/time.h>

#include "monikor.h"

#define MOD_NAME "process"

int poll_processes_metrics(monikor_t *mon, struct timeval *clock);

#endif /* end of include guard: PROCESS_H_ */
