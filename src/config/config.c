#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <yaml.h>

#include "config.h"
#include "logger.h"


monikor_config_t *monikor_config_new(void) {
  monikor_config_t *cfg;

  if (!(cfg = malloc(sizeof(*cfg))))
    return NULL;
  cfg->full_config = monikor_config_dict_new();
  cfg->config_path = MONIKOR_DEFAULT_CONFIG_PATH;
  cfg->user = NULL;
  cfg->group = NULL;
  cfg->run_dir = MONIKOR_DEFAULT_RUN_DIR;
  cfg->daemonize = 0;
  cfg->server.url = NULL;
  cfg->server.proxy_url = NULL;
  cfg->modules.path = NULL;
  cfg->modules.config_path = NULL;
  cfg->modules.modules = NULL;
  cfg->quanta_token = NULL;
  cfg->hostid = NULL;
  cfg->listen.path = NULL;
  cfg->listen.user = NULL;
  cfg->listen.group = NULL;
  cfg->listen.mode = MONIKOR_DEFAULT_LISTEN_MODE;
  cfg->logger.level = LOG_NOTICE;
  cfg->logger.file = NULL;
  cfg->max_cache_size = MONIKOR_DEFAULT_MAX_CACHE_SIZE;
  return cfg;
}

void monikor_config_free(monikor_config_t *config) {
  if (!config)
    return;
  monikor_config_dict_free(config->full_config);
  strl_free(config->modules.modules);
  free(config);
}

static int monikor_load_config_dict(const char *path, monikor_config_dict_t *cfg) {
  FILE *config_fh;

  if (!(config_fh = fopen(path, "r"))) {
    monikor_log(LOG_ERR, "error opening configuration file %s: %s\n",
      path, strerror(errno));
    return 1;
  }
  monikor_log(LOG_DEBUG, "loading configuration file %s\n", path);
  if (monikor_parse_config_file(config_fh, cfg)) {
    monikor_log(LOG_ERR, "error parsing configuration file %s\n", path);
    fclose(config_fh);
    return 1;
  }
  fclose(config_fh);
  return 0;
}

monikor_config_t *monikor_load_config(char *config_path) {
  monikor_config_t *cfg;

  if (!(cfg = monikor_config_new())) {
    monikor_log(LOG_ERR, "cannot allocate memory\n");
    return NULL;
  }
  if (config_path)
    cfg->config_path = config_path;
  if (monikor_load_config_dict(cfg->config_path, cfg->full_config)) {
    monikor_config_free(cfg);
    return NULL;
  }
  monikor_setup_config(cfg);
  return cfg;
}

monikor_config_dict_t *monikor_load_mod_config(char *mod_config_path, char *mod_name) {
  monikor_config_dict_t *cfg = NULL;
  char *config_path;

  if (!(cfg = monikor_config_dict_new())
  || asprintf(&config_path, "%s/%s"MONIKOR_CONFIG_EXT, mod_config_path, mod_name) == -1) {
    monikor_config_dict_free(cfg);
    monikor_log(LOG_ERR, "cannot allocate memory to load module %s config\n", mod_name);
    return NULL;
  }
  if (monikor_load_config_dict(config_path, cfg)) {
    monikor_config_dict_free(cfg);
    free(config_path);
    return NULL;
  }
  free(config_path);
  return cfg;
}
