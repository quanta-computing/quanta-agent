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
}

static void _monikor_load_one_module(char *name, monikor_t *mon) {
  monikor_mod_t *mod;

  mod = monikor_load_module(name, mon->config->modules.path);
  mod->config = monikor_load_mod_config(mon->config->modules.config_path, mod->name);
  set_interval_and_clock(mon, mod);
  mod->data = mod->init(mon, mod->config);
  mon->modules.modules[mon->modules.count] = mod;
  mon->modules.count++;
}

int monikor_load_all_modules(monikor_t *mon) {
  strl_apply_data(mon->config->modules.modules,
    (void (*)(char *, void *))_monikor_load_one_module,
    (void *)mon
  );
  return 0;
}
