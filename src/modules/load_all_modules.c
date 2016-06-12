#include <sys/time.h>

#include "monikor.h"
#include "module.h"
#include "strl.h"


static void set_interval_and_clock(monikor_t *mon,monikor_mod_t *mod) {
  char *interval;
  char *end;

  gettimeofday(&mod->last_clock, NULL);
  interval = monikor_config_dict_get_scalar(mod->config, "interval");
  if (!interval) {
    mod->poll_interval = mon->config->poll_interval;
  } else {
    mod->poll_interval = (int)strtol(interval, &end, 10);
    if (*end || mod->poll_interval < 0) {
      monikor_log(LOG_WARNING, "Wrong interval value '%s' in %s module configuration\n",
        interval, mod->name);
      mod->poll_interval = mon->config->poll_interval;
    } else if (mod->poll_interval > mon->config->poll_interval) {
      monikor_log(LOG_WARNING, "Interval for %s module is greater than global interval\n", mod->name);
      mod->poll_interval = mon->config->poll_interval;
    }
  }
  mod->last_clock.tv_sec -= mod->poll_interval;
}

static void monikor_load_one_module(char *name, void *data) {
  monikor_t *mon = (monikor_t *)data;
  monikor_mod_t *mod;
  monikor_config_dict_t *config;
  char *enabled;
  char *mod_name;

  if (!(config = monikor_load_mod_config(mon->config->modules.config_path, name)))
    return;
  enabled = monikor_config_dict_get_scalar(config, "enabled");
  mod_name = monikor_config_dict_get_scalar(config, "module");
  mod_name = mod_name ? mod_name : name;
  if ((enabled && (!strcmp(enabled, "no") || !strcmp(enabled, "false")))
  || !(mod = monikor_load_module(mod_name, mon->config->modules.path))) {
    monikor_log(LOG_INFO, "Module %s disabled\n", name);
    monikor_config_dict_free(config);
    return;
  }
  mod->config = config;
  set_interval_and_clock(mon, mod);
  mod->data = mod->init(mon, mod->config);
  mon->modules.modules[mon->modules.count] = mod;
  mon->modules.count++;
}

int monikor_load_all_modules(monikor_t *mon) {
  strl_apply_data(mon->config->modules.modules, monikor_load_one_module, (void *)mon);
  return 0;
}
