#include <stdlib.h>

#include "config.h"
#include "logger.h"

static const struct {
  char *lower;
  char *upper;
} log_level_mappings[] = {
  {"emerg", "EMERG"},
  {"alert", "ALERT"},
  {"critical", "CRITICAL"},
  {"error", "ERROR"},
  {"warning", "WARNING"},
  {"notice", "NOTICE"},
  {"info", "INFO"},
  {"debug", "DEBUG"},
  {NULL, NULL}
};

static int monikor_setup_config_log_level(monikor_config_t *cfg) {
  char *level;

  cfg->log_level = LOG_INFO;
  if (!(level = monikor_config_dict_get_scalar(cfg->full_config, "log_level")))
    return 1;
  for (int i = 0; log_level_mappings[i].lower; i++)
    if (!strcmp(level, log_level_mappings[i].lower)
    || !strcmp(level, log_level_mappings[i].upper)) {
      cfg->log_level = i;
      return 0;
    }
  return 1;
}


static int _check_interval(const char *interval) {
  if (!interval)
    return 0;
  while (*interval) {
    if (*interval < '0' || *interval > '9')
      return 1;
    interval++;
  }
  return 0;
}

static int monikor_setup_config_interval(monikor_config_t *cfg) {
  char *interval;

  interval = monikor_config_dict_get_scalar(cfg->full_config, "interval");
  if (!_check_interval(interval))
    cfg->poll_interval = interval ? atoi(interval) : MONIKOR_DEFAULT_POLL_INTERVAL;
  else {
    monikor_log(LOG_WARNING, "Wrong value '%s' for poll_interval, using default.\n", interval);
    cfg->poll_interval = MONIKOR_DEFAULT_POLL_INTERVAL;
  }
  return 0;
}

static int monikor_setup_config_modules(monikor_config_t *cfg) {
  char *mod_path;

  mod_path = monikor_config_dict_get_scalar(cfg->full_config, "modules_path");
  cfg->modules.path = (mod_path ? strdup(mod_path) : strdup(MONIKOR_DEFAULT_MODULES_PATH));
  cfg->modules.modules = monikor_config_dict_get_list(cfg->full_config, "modules");
  return 0;
}

static int monikor_setup_config_server(monikor_config_t *cfg) {
  char *port;

  cfg->server.host = strdup(monikor_config_dict_get_scalar(cfg->full_config, "server.host"));
  port = monikor_config_dict_get_scalar(cfg->full_config, "server.port");
  cfg->server.port = port ? atoi(port) : MONIKOR_SERVER_DEFAULT_PORT;
  return 0;
}

int monikor_setup_config(monikor_config_t *cfg) {
  monikor_setup_config_modules(cfg);
  monikor_setup_config_server(cfg);
  monikor_setup_config_log_level(cfg);
  monikor_setup_config_interval(cfg);
  return 0;
}
