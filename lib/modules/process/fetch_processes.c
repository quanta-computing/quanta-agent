#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#include "process.h"


static int looks_like_pid_dir(const struct dirent *entry) {
  if (entry->d_type != DT_DIR)
    return 0;
  for (size_t i = 0; entry->d_name[i]; i++)
    if (!isdigit(entry->d_name[i]))
      return 0;
  return 1;
}

static process_t *fetch_process(const char *pid) {
  char buf[1024];
  char *name;
  char *end;
  int rd;
  int fd;

  sprintf(buf, "/proc/%s/stat", pid);
  if ((fd = open(buf, O_RDONLY)) == -1
  || (rd = read(fd, buf, sizeof(buf) - 1)) == -1)
    goto err;
  close(fd);
  buf[rd] = 0;
  if (!(name = strchr(buf, '('))
  || !(end = strchr(name, ')')))
    goto err;
  *end = 0;
  return process_new(name + 1, atoi(pid));

err:
  close(fd);
  return NULL;
}

process_list_t *fetch_processes(void) {
  process_list_t *list = NULL;
  process_t *process;
  DIR *dir;
  struct dirent entry;
  struct dirent *res;

  if (!(dir = opendir("/proc"))
  || !(list = process_list_new())) {
    closedir(dir);
    return NULL;
  }
  while (!readdir_r(dir, &entry, &res) && res) {
    if (!looks_like_pid_dir(&entry))
      continue;
    if (!(process = fetch_process(entry.d_name)))
      monikor_log_mod(LOG_WARNING, MOD_NAME, "Cannot get process info for pid %s\n", entry.d_name);
    else
      process_list_push(list, process);
  }
  closedir(dir);
  return list;
}
