#include <stdlib.h>
#include <time.h>

#include "monikor.h"
#include "varnish4.h"


void *varnish_init(monikor_t *mon, monikor_config_dict_t *config) {
  varnish_module_t *mod;

  (void)mon;
  if (!(mod = malloc(sizeof(*mod))))
    return NULL;
  mod->mon = mon;
  if (!(mod->vd = VSM_New())) {
    free(mod);
    return NULL;
  }

  mod->instance = monikor_config_dict_get_scalar(config, "varnish.instance");
  if (mod->instance) {
    if (strlen(mod->instance) >= MONIKOR_VARNISH_MAX_INSTANCE_LENGTH) {
      mod->instance[MONIKOR_VARNISH_MAX_INSTANCE_LENGTH - 1] = 0;
      monikor_log_mod(LOG_WARNING, MOD_NAME, "Instance name too long, stripped to %d chars\n",
        MONIKOR_VARNISH_MAX_INSTANCE_LENGTH);
    }
    VSM_n_Arg(mod->vd, mod->instance);
  }
  return (void *)mod;
}

void varnish_cleanup(monikor_t *mon, void *data) {
  varnish_module_t *mod = (varnish_module_t *)data;

  (void)mon;
  VSM_Close(mod->vd);
  VSM_Delete(mod->vd);
  free(mod);
}

int varnish_poll(monikor_t *mon, void *data) {
  struct timeval now;

  (void)mon;
  gettimeofday(&now, NULL);
  if (!data) {
    monikor_log_mod(LOG_ERR, MOD_NAME, "Invalid context data.\n");
    return -1;
  }
  return varnish_poll_metrics((varnish_module_t *)data, &now);
}
