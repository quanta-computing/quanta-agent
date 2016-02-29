#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "logger.h"

static int _monikor_log_level = LOG_INFO;
static int _log_fd = -1;


/*
** If prio is MONIKOR_LOG_DEFAULT, we setup the logger to use stderr. This is useful to wait
** until we read the config and setup the logger properly again.
** if log_file is not NULL, we try to open the file.
** We always fallback to syslogging if anything goes wrong
*/
void monikor_logger_init(int prio, const char *log_file) {
  if (prio != MONIKOR_LOG_DEFAULT)
    _monikor_log_level = prio;
  if (prio == MONIKOR_LOG_DEFAULT
  || (log_file && !strcmp(log_file, "stderr"))) {
    _log_fd = 2;
  }
  else if (log_file) {
    if ((_log_fd = open(log_file, O_WRONLY|O_APPEND|O_CREAT)) == -1) {
      fprintf(stderr, "Cannot open log file %s, fallbacking to syslog\n", log_file);
    }
  }
  if (_log_fd == -1) {
    closelog();
    openlog("monikor", LOG_PID, LOG_DAEMON);
  }
}

void monikor_logger_cleanup(void) {
  if (_log_fd != 2 && _log_fd != -1)
    close(_log_fd);
  else
    closelog();
}

int monikor_vlog(int prio, const char *message, va_list ap) {
  if (prio == MONIKOR_LOG_DEFAULT)
    prio = _monikor_log_level;
  if (prio < MONIKOR_LOG_MIN_PRIO || prio > MONIKOR_LOG_MAX_PRIO)
    return -1;
  if (prio > _monikor_log_level)
    return 0;
  if (_log_fd == -1)
    vsyslog(prio, message, ap);
  else
    vdprintf(_log_fd, message, ap);
  return 0;
}

int monikor_log(int prio, const char *message, ...) {
  int ret;
  va_list ap;

  va_start(ap, message);
  ret = monikor_vlog(prio, message, ap);
  va_end(ap);
  return ret;
}

static char *_format_mod_log_message(const char *mod_name, const char *message) {
  char *full_msg;

  if (!(full_msg = malloc(strlen(mod_name) + strlen(MONIKOR_LOG_SEP) + strlen(message) + 1)))
    return NULL;
  strcpy(full_msg, mod_name);
  strcat(full_msg, MONIKOR_LOG_SEP);
  strcat(full_msg, message);
  return full_msg;
}

int monikor_vlog_mod(int prio, const char *mod_name, const char *message, va_list ap) {
  int ret;
  char *full_msg;

  if (!(full_msg = _format_mod_log_message(mod_name, message)))
    return -1;
  ret = monikor_vlog(prio, full_msg, ap);
  free(full_msg);
  return ret;
}

int monikor_log_mod(int prio, const char *mod_name, const char *message, ...) {
  int ret;
  va_list ap;
  char *full_msg;

  va_start(ap, message);
  if (!(full_msg = _format_mod_log_message(mod_name, message)))
    return -1;
  ret = monikor_vlog(prio, full_msg, ap);
  va_end(ap);
  free(full_msg);
  return ret;
}
