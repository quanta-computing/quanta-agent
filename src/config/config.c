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
  cfg->server_url = NULL;
  cfg->modules.path = NULL;
  cfg->quanta_token = NULL;
  cfg->hostid = NULL;
  cfg->unix_sock_path = NULL;
  cfg->log_level = LOG_NOTICE;
  cfg->max_cache_size = MONIKOR_DEFAULT_MAX_CACHE_SIZE;
  return cfg;
}

void monikor_config_free(monikor_config_t *config) {
  monikor_config_dict_free(config->full_config);
  free(config);
}

monikor_config_t *monikor_load_config(char *config_path) {
  monikor_config_t *cfg;
  FILE *config_fh;

  if (!(cfg = monikor_config_new())) {
    monikor_log(LOG_ERR, "cannot allocate memory\n");
    return NULL;
  }
  if (config_path && strcmp(config_path, cfg->config_path)) {
    cfg->config_path = config_path;
  }
  if (!(config_fh = fopen(cfg->config_path, "r"))) {
    monikor_log(LOG_ERR, "error opening configuration file %s: %s\n",
      cfg->config_path, strerror(errno));
    return NULL;
  }
  monikor_log(LOG_DEBUG, "loading configuration file %s\n", cfg->config_path);
  if (monikor_parse_config_file(config_fh, cfg)) {
    monikor_log(LOG_ERR, "error parsing configuration file %s\n", cfg->config_path);
    fclose(config_fh);
    return NULL;
  }
  monikor_setup_config(cfg);
  fclose(config_fh);
  return cfg;
}
