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


static int _is_number(const char *str) {
  if (!str)
    return 0;
  while (*str) {
    if (*str < '0' || *str > '9')
      return 0;
    str++;
  }
  return 1;
}

static int monikor_setup_config_interval(monikor_config_t *cfg) {
  char *interval;

  interval = monikor_config_dict_get_scalar(cfg->full_config, "interval");
  if (_is_number(interval))
    cfg->poll_interval = atoi(interval);
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
  char *url;
  char *timeout;

  url = monikor_config_dict_get_scalar(cfg->full_config, "server_url");
  cfg->server_url = url ? strdup(url) : NULL;
  timeout = monikor_config_dict_get_scalar(cfg->full_config, "server_timeout");
  if (_is_number(timeout)) {
    cfg->server_timeout = atoi(timeout);
  } else {
    monikor_log(LOG_WARNING, "Wrong value '%s' for server_timeout, using default.\n", timeout);
    cfg->server_timeout= MONIKOR_DEFAULT_SERVER_TIMEOUT;
  }
  return 0;
}

static int monikor_setup_config_unix_sock_path(monikor_config_t *cfg) {
  char *unix_sock_path;

  unix_sock_path = monikor_config_dict_get_scalar(cfg->full_config, "unix_socket_path");
  cfg->unix_sock_path = (unix_sock_path ? strdup(unix_sock_path) : NULL);
  return 0;
}

// Check for NULL and size
static int monikor_setup_quanta_token(monikor_config_t *cfg) {
  char *token;

  token = monikor_config_dict_get_scalar(cfg->full_config, "quanta_token");
  cfg->quanta_token = token ? strdup(token) : NULL;
  return 0;
}


// Check for NULL and size
static int monikor_setup_hostid(monikor_config_t *cfg) {
  char *hostid;

  hostid = monikor_config_dict_get_scalar(cfg->full_config, "hostid");
  cfg->hostid = hostid ? strdup(hostid) : NULL;
  return 0;
}

int monikor_setup_config(monikor_config_t *cfg) {
  monikor_setup_config_modules(cfg);
  monikor_setup_config_server(cfg);
  monikor_setup_config_log_level(cfg);
  monikor_setup_config_interval(cfg);
  monikor_setup_config_unix_sock_path(cfg);
  monikor_setup_quanta_token(cfg);
  monikor_setup_hostid(cfg);
  return 0;
}
