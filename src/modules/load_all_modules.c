#include "monikor.h"
#include "module.h"
#include "strl.h"

static void _monikor_load_one_module(char *name, monikor_t *mon) {
  monikor_mod_t *mod;

  mod = monikor_load_module(name, mon->config->modules.path);
  mod->data = mod->init(mon);
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
