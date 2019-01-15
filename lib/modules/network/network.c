#include <stdlib.h>
#include <time.h>

#include "monikor.h"
#include "network.h"


void *network_init(monikor_t *mon, monikor_config_dict_t *config) {
  monikor_net_mod_t *mod;

  (void)mon;
  if (!(mod = malloc(sizeof(*mod))))
    return NULL;
  mod->ifaces = monikor_config_dict_get_list(config, "network.interfaces");
  return mod;
}

void network_cleanup(monikor_t *mon, void *data) {
  monikor_net_mod_t *mod = (monikor_net_mod_t *)data;

  (void)mon;
  free(mod);
}

int network_poll(monikor_t *mon, void *data) {
  monikor_net_mod_t *mod = (monikor_net_mod_t *)data;
  struct timeval now;
  int i = 0;

  if (!mod) {
    monikor_log_mod(LOG_ERR, MOD_NAME, "Invalid context data.\n");
    return -1;
  }
  gettimeofday(&now, NULL);
  i += poll_network_metrics(mon, &now, mod);
  i += poll_tcp_metrics(mon, &now);
  return i;
}
