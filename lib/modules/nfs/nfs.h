#ifndef nfs_H_
#define nfs_H_

#include "monikor.h"

#define MOD_NAME "nfs"

int nfs_fetch_metrics(monikor_t *mon, struct timeval *clock);

#endif /* end of include guard: nfs_H_ */
