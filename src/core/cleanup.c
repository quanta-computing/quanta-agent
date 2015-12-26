#include "monikor.h"

static void _cleanup_modules(monikor_t *mon) {
  for (size_t i = 0; i < mon->modules.count; i++)
    monikor_mod_destroy(mon->modules.modules[i]);
  free(mon->modules.modules);
}

void monikor_cleanup(monikor_t *mon) {
  _cleanup_modules(mon);
}
