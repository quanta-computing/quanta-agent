#include "monikor.h"
#include "nfs.h"

static const char *metrics[] = {
  "", "", // Skip 2 first values
  "nfs.client.v3.getattr",
  "nfs.client.v3.setattr",
  "nfs.client.v3.lookup",
  "nfs.client.v3.access",
  "nfs.client.v3.readlink",
  "nfs.client.v3.read",
  "nfs.client.v3.write",
  "nfs.client.v3.create",
  "nfs.client.v3.mkdir",
  "nfs.client.v3.symlink",
  "nfs.client.v3.mknod",
  "nfs.client.v3.remove",
  "nfs.client.v3.rmdir",
  "nfs.client.v3.rename",
  "nfs.client.v3.link",
  "nfs.client.v3.readdir",
  "nfs.client.v3.readdirplus",
  "nfs.client.v3.fsstat",
  "nfs.client.v3.fsinfo",
  "nfs.client.v3.pathconf",
  "nfs.client.v3.commit",
  NULL
};

static int fetch_metrics(monikor_t *mon, struct timeval *clock, char *proc) {
  uint64_t value;
  int n;

  proc += strlen("proc3 ");
  for (size_t i = 0; metrics[i]; i++) {
    value = (uint64_t)strtoull(proc, &proc, 10);
    if (*metrics[i])
      continue;
    n += monikor_metric_push(mon, monikor_metric_integer(metrics[i],
      clock, value, MONIKOR_METRIC_TIMEDELTA));
  }
  return n;
}

int nfs_fetch_metrics(monikor_t *mon, struct timeval *clock) {
  char *nfsstat;
  char *proc;

  if (!(nfsstat = monikor_read_file("/proc/net/rpc/nfs"))
  || !(proc = strstr(nfsstat, "proc3"))) {
    return 0;
  }
  return fetch_metrics(mon, clock, proc);
}
