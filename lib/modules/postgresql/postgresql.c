#include <stdlib.h>
#include <time.h>

#include "monikor.h"
#include "postgresql.h"


void *postgresql_init(monikor_t *mon, monikor_config_dict_t *config) {
  postgresql_module_t *mod;

  (void)mon;
  if (!(mod = malloc(sizeof(*mod))))
    goto err;
  mod->host = monikor_config_dict_get_scalar(config, "postgresql.host");
  if (!mod->host || !*mod->host)
    mod->host = POSTGRESQL_DEFAULT_HOST;
  mod->port = monikor_config_dict_get_scalar(config, "postgresql.port");
  if (!mod->port || !*mod->port)
    mod->port = POSTGRESQL_DEFAULT_PORT;
  mod->user = monikor_config_dict_get_scalar(config, "postgresql.user");
  mod->password = monikor_config_dict_get_scalar(config, "postgresql.password");
  mod->database = monikor_config_dict_get_scalar(config, "postgresql.database");
  if (!mod->user) {
    monikor_log_mod(LOG_ERR, MOD_NAME, "missing user in config\n");
    goto err;
  }
  if (!mod->password) {
    monikor_log_mod(LOG_ERR, MOD_NAME, "missing password in config\n");
    goto err;
  }
  if (!mod->database) {
    monikor_log_mod(LOG_ERR, MOD_NAME, "missing database in config\n");
    goto err;
  }
  if (strlen(mod->database) >= MONIKOR_POSTGRES_MAX_INSTANCE_LENGTH) {
    mod->database[MONIKOR_POSTGRES_MAX_INSTANCE_LENGTH - 1] = 0;
    monikor_log_mod(LOG_WARNING, MOD_NAME, "database name too long, stripped to %d chars\n",
      MONIKOR_POSTGRES_MAX_INSTANCE_LENGTH);
  }
  return mod;

err:
  free(mod);
  return NULL;
}

void postgresql_cleanup(monikor_t *mon, void *data) {
  (void)mon;
  free(data);
}

int postgresql_poll(monikor_t *mon, void *data) {
  postgresql_module_t *mod = (postgresql_module_t *)data;
  struct timeval now;

  if (!mod) {
    monikor_log_mod(LOG_ERR, MOD_NAME, "invalid context data\n");
    return -1;
  }
  gettimeofday(&now, NULL);
  return (postgresql_poll_metrics(mon, &now, mod));
}
