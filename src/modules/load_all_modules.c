#include "module.h"
#include "strl.h"
#include "monikor.h"

static void _monikor_load_one_module(char *name, monikor_t *mon) {
  mon->modules.modules[mon->modules.count] = monikor_load_module(name, mon->config->modules.path);
  mon->modules.modules[mon->modules.count]->init();
  mon->modules.count++;
}

// We shoud find a way to handle errors
int monikor_load_all_modules(monikor_t *mon) {
  strl_apply_data(mon->config->modules.modules,
    (void (*)(char *, void *))_monikor_load_one_module,
    (void *)mon
  );
  return 0;
}
