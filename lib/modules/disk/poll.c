#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/statvfs.h>

#include "monikor.h"
#include "disk.h"

static const char * dev_metrics[] = {
  "", "", "bytes.read", "",
  "", "", "bytes.write", "",
  "", "io.spent_ms", "",
  NULL
};

static int process_one_disk_dev_stat(monikor_t *mon, struct timeval *clock, char *dev, char *stats,
uint64_t sector_size) {
  uint64_t val;
  char *stat;
  char *next;
  char metric_name[512];
  char *metric_base_end;
  int n = 0;

  sprintf(metric_name, "disk.device.%s.", dev);
  metric_base_end = metric_name + strlen(metric_name);
  stat = stats;
  for (size_t i = 0; dev_metrics[i]; i++, stat = next) {
    val = strtoull(stat, &next, 10);
    if (next == stat)
      break;
    if (!*dev_metrics[i])
      continue;
    if (!strncmp(dev_metrics[i], "bytes.", 6))
      val *= sector_size;
    strcpy(metric_base_end, dev_metrics[i]);
    monikor_metric_push(mon,
      monikor_metric_integer(metric_name, clock, val, MONIKOR_METRIC_TIMEDELTA));
    n++;
  }
  return n;
}

static int is_monitored_device(const char *dev) {
  return strlen(dev) == 3
    && (dev[0] == 'h' || dev[0] == 's')
    && dev[1] == 'd'
    && (dev[2] >= 'a' && dev[2] <= 'z');
}

static int fetch_one_disk_dev_metrics(monikor_t *mon, struct timeval *clock, char *part) {
  char *dev;
  char *stats = NULL;
  char *sector_size = NULL;
  char file_name[256];
  int n = 0;

  if (!(dev = strrchr(part, ' '))
  || !is_monitored_device(dev + 1))
    goto end;
  sprintf(file_name, "/sys/block/%s/queue/hw_sector_size", dev + 1);
  if (!(sector_size = monikor_read_file(file_name)))
    goto end;
  sprintf(file_name, "/sys/block/%s/stat", dev + 1);
  if (!(stats = monikor_read_file(file_name)))
    goto end;
  n = process_one_disk_dev_stat(mon, clock, dev + 1, stats, strtoull(sector_size, NULL, 10));

end:
  free(sector_size);
  free(stats);
  return n;
}

static int fetch_disk_dev_metrics(monikor_t *mon, struct timeval *clock, char *parts) {
  char *next;
  int n = 0;

  while (*parts) {
    next = strchr(parts, '\n');
    if (!next)
      return -1;
    *next = 0;
    n += fetch_one_disk_dev_metrics(mon, clock, parts);
    parts = next + 1;
  }
  return n;
}

static const char *monitored_fs[] = {
  "btrfs", "ext2", "ext3", "ext4", "jfs", "reiser", "xfs", "ffs", "ufs", "jfs", "jfs2", "vxfs",
  "hfs", "ntfs", "fat3", NULL
};

static int is_monitored_fs(const char *fstype) {
  for (size_t i = 0; monitored_fs[i]; i++) {
    if (!strcmp(fstype, monitored_fs[i]))
      return 1;
  }
  return 0;
}

static int fetch_one_disk_fs_metrics(monikor_t *mon, struct timeval *clock, char *mount) {
  char *path;
  char *fstype;
  char *opts;
  struct statvfs stat;
  char metric_name[1280];
  char *metric_base_end;

  if (!(path = strchr(mount, ' '))
  || !(fstype = strchr(path + 1, ' '))
  || !(opts = strchr(fstype + 1, ' ')))
    return 0;
  *fstype = 0;
  *opts = 0;
  if (!is_monitored_fs(fstype + 1))
    return 0;
  if (statvfs(path + 1, &stat)) {
    monikor_log_mod(LOG_ERR, MOD_NAME, "Cannot perform statvfs(2) on %s: %s\n",
      path, strerror(errno));
    return 0;
  }
  if (snprintf(metric_name, 1024, "disk.fs.%s.", path + 1) >= 1024)
    return 0;
  metric_base_end = metric_name + strlen(metric_name);

  strcpy(metric_base_end, "bytes.total");
  monikor_metric_push(mon, monikor_metric_integer(metric_name, clock,
    (uint64_t)(stat.f_frsize * stat.f_blocks), 0));
  strcpy(metric_base_end, "bytes.free");
  monikor_metric_push(mon, monikor_metric_integer(metric_name, clock,
    (uint64_t)(stat.f_frsize * stat.f_bfree), 0));
  strcpy(metric_base_end, "bytes.available");
  monikor_metric_push(mon, monikor_metric_integer(metric_name, clock,
    (uint64_t)(stat.f_frsize * stat.f_bavail), 0));

  strcpy(metric_base_end, "inodes.total");
  monikor_metric_push(mon, monikor_metric_integer(metric_name, clock, (uint64_t)stat.f_files, 0));
  strcpy(metric_base_end, "inodes.free");
  monikor_metric_push(mon, monikor_metric_integer(metric_name, clock, (uint64_t)stat.f_ffree, 0));
  strcpy(metric_base_end, "inodes.available");
  monikor_metric_push(mon, monikor_metric_integer(metric_name, clock, (uint64_t)stat.f_favail, 0));

  return 6;
}

static int fetch_disk_fs_metrics(monikor_t *mon, struct timeval *clock, char *mounts) {
  char *next;
  int n = 0;

  while (*mounts) {
    next = strchr(mounts, '\n');
    if (!next)
      return -1;
    *next = 0;
    n += fetch_one_disk_fs_metrics(mon, clock, mounts);
    mounts = next + 1;
  }
  return n;
}

int poll_disk_metrics(monikor_t *mon, struct timeval *clock) {
  char *mounts;
  char *parts;
  int n;

  n = 0;
  if ((mounts = monikor_read_file("/proc/mounts")))
    n += fetch_disk_fs_metrics(mon, clock, mounts);
  if ((parts = monikor_read_file("/proc/partitions")))
    n += fetch_disk_dev_metrics(mon, clock, parts);
  free(mounts);
  free(parts);
  return n;
}
