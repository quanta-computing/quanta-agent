#ifndef disk_H_
#define disk_H_

#include "monikor.h"

#define MOD_NAME "disk"

int poll_disk_metrics(monikor_t *mon, struct timeval *clock);

#endif /* end of include guard: disk_H_ */
