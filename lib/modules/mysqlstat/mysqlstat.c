#include <stdlib.h>
#include <time.h>

#include "monikor.h"
#include "mysqlstat.h"


void *mysqlstat_init(monikor_t *mon, monikor_config_dict_t *cfg) {
  monikor_mysql_config_t *config;
  char *port;

  if (!(config = malloc(sizeof(*config))))
    return NULL;
  config->host = monikor_config_dict_get_scalar(cfg, "mysql.host");
  config->user = monikor_config_dict_get_scalar(cfg, "mysql.user");
  config->password = monikor_config_dict_get_scalar(cfg, "mysql.password");
  port = monikor_config_dict_get_scalar(cfg, "mysql.port");
  if (!config->user || !config->password) {
    monikor_log_mod(LOG_ERR, MOD_NAME, "Missing username or password in configuration\n");
    free(config);
    return NULL;
  }
  if (!config->host)
    config->host = MONIKOR_MYSQL_DEFAULT_HOST;
  if (!port)
    config->port = 0;
  else
    config->port = atoi(port);
  config->timeout = MONIKOR_MYSQL_TIMEOUT;
  if (mon->config->poll_interval > 0 && mon->config->poll_interval < MONIKOR_MYSQL_TIMEOUT)
    config->timeout = (unsigned int)mon->config->poll_interval;
  return (void *)config;
}

void mysqlstat_cleanup(monikor_t *mon, void *data) {
  (void)mon;
  free(data);
}

int mysqlstat_poll(monikor_t *mon, void *data) {
  struct timeval now;

  if (!data)
    return -1;
  gettimeofday(&now, NULL);
  return mysqlstat_poll_metrics(mon, &now, (monikor_mysql_config_t *)data);
}
