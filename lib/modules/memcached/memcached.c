#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include "monikor.h"
#include "io_handler.h"
#include "memcached.h"


void *memcached_init(monikor_t *mon, monikor_config_dict_t *config) {
  memcached_module_t *mod;

  (void)mon;
  if (!(mod = malloc(sizeof(*mod))))
    return NULL;
  mod->mon = mon;
  mod->io_handler = NULL;
  mod->host = monikor_config_dict_get_scalar(config, "memcached.host");
  mod->port = monikor_config_dict_get_scalar(config, "memcached.port");
  mod->instance = monikor_config_dict_get_scalar(config, "memcached.instance");
  if (mod->instance && strlen(mod->instance) >= MONIKOR_MEMCACHED_MAX_INSTANCE_LENGTH) {
    mod->instance[MONIKOR_MEMCACHED_MAX_INSTANCE_LENGTH - 1] = 0;
    monikor_log_mod(LOG_WARNING, MOD_NAME, "Instance name too long, stripped to %d chars\n",
      MONIKOR_MEMCACHED_MAX_INSTANCE_LENGTH);
  }
  if (!mod->host)
    mod->host = MONIKOR_MEMCACHED_DEFAULT_HOST;
  if (!mod->port)
    mod->port = MONIKOR_MEMCACHED_DEFAULT_PORT;
  return mod;
}

void memcached_cleanup(monikor_t *mon, void *data) {
  memcached_module_t *mod = (memcached_module_t *)data;

  if (mod->io_handler) {
    monikor_unregister_io_handler(mon, mod->io_handler);
    free(mod->io_handler->data);
    if (mod->io_handler->fd != -1)
      close(mod->io_handler->fd);
    free(mod->io_handler);
  }
  free(mod);
}

int memcached_poll(monikor_t *mon, void *data) {
  memcached_module_t *mod = (memcached_module_t *)data;

  if (!mod)
    return -1;
  if (mod->io_handler) {
    monikor_unregister_io_handler(mon, mod->io_handler);
    if (mod->io_handler->fd != -1)
      close(mod->io_handler->fd);
    free(mod->io_handler);
    mod->io_handler = NULL;
  }
  mod->io_handler = monikor_net_exchange(mod->host, mod->port, "stats\n",
    &memcached_poll_metrics, (void *)mod);
  if (!mod->io_handler) {
    monikor_log_mod(LOG_ERR, MOD_NAME, "Cannot connect to %s:%s: %s\n",
      mod->host, mod->port, strerror(errno));
    return -1;
  }
  monikor_register_io_handler(mon, mod->io_handler);
  return 0;
}
