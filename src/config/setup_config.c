#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "config.h"
#include "logger.h"
#include "strl.h"

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
    if (interval)
      monikor_log(LOG_WARNING, "Wrong value '%s' for poll_interval, using default.\n", interval);
    cfg->poll_interval = MONIKOR_DEFAULT_POLL_INTERVAL;
  }
  return 0;
}

static int is_module_config(const char *name) {
  size_t name_len = strlen(name);
  size_t ext_len = strlen(MONIKOR_CONFIG_EXT);

  if (name_len <= ext_len)
    return 0;
  return !strcmp(name + name_len - strlen(MONIKOR_CONFIG_EXT), MONIKOR_CONFIG_EXT);
}

static int monikor_setup_config_modules_path(monikor_config_t *cfg) {
  char *mod_path;
  char *config_path;
  size_t path_len;
  size_t conf_path_len;

  mod_path = monikor_config_dict_get_scalar(cfg->full_config, "modules.path");
  config_path = monikor_config_dict_get_scalar(cfg->full_config, "modules.config_path");
  cfg->modules.path = mod_path ? mod_path : MONIKOR_DEFAULT_MODULES_PATH;
  cfg->modules.config_path = config_path ? config_path : MONIKOR_DEFAULT_MODULES_CONFIG_PATH;
  path_len = strlen(cfg->modules.path);
  conf_path_len = strlen(cfg->modules.config_path);
  if (cfg->modules.path[path_len - 1] == '/')
    cfg->modules.path[path_len - 1] = 0;
  if (cfg->modules.config_path[conf_path_len - 1] == '/')
    cfg->modules.config_path[conf_path_len - 1] = 0;
  return 0;
}

static int monikor_setup_config_modules(monikor_config_t *cfg) {
  DIR *mod_dir;
  struct dirent entry;
  struct dirent *res;

  monikor_setup_config_modules_path(cfg);
  if (!(cfg->modules.modules = strl_new())
  || !(mod_dir = opendir(cfg->modules.config_path))) {
    monikor_log(LOG_ERR, "Cannot get modules list\n");
    return 1;
  }
  while (!readdir_r(mod_dir, &entry, &res) && res) {
    if (is_module_config(entry.d_name)) {
      char *ext = strstr(entry.d_name, MONIKOR_CONFIG_EXT);
      *ext = 0;
      strl_push(cfg->modules.modules, entry.d_name);
    }
  }
  closedir(mod_dir);
  return 0;
}

static int monikor_setup_config_server(monikor_config_t *cfg) {
  char *url;
  char *timeout;

  url = monikor_config_dict_get_scalar(cfg->full_config, "server.url");
  cfg->server_url = url;
  timeout = monikor_config_dict_get_scalar(cfg->full_config, "server.timeout");
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
  cfg->unix_sock_path = unix_sock_path;
  return 0;
}

// Check for NULL and size
static int monikor_setup_quanta_token(monikor_config_t *cfg) {
  char *token;

  token = monikor_config_dict_get_scalar(cfg->full_config, "quanta_token");
  cfg->quanta_token = token;
  return 0;
}


// Check for NULL and size
static int monikor_setup_hostid(monikor_config_t *cfg) {
  char *hostid;

  hostid = monikor_config_dict_get_scalar(cfg->full_config, "hostid");
  cfg->hostid = hostid;
  return 0;
}

static int monikor_setup_cache_size(monikor_config_t *cfg) {
  char *size;

  size = monikor_config_dict_get_scalar(cfg->full_config, "max_cache_size_mb");
  if (_is_number(size))
    cfg->max_cache_size = atoi(size) * 1024 * 1024;
  else {
    if (size)
      monikor_log(LOG_WARNING, "Wrong value '%s' for max_cache_size_mb, using default.\n", size);
    cfg->max_cache_size = MONIKOR_DEFAULT_MAX_CACHE_SIZE;
  }
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
  monikor_setup_cache_size(cfg);
  return 0;
}
