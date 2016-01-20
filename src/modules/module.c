#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "module.h"


monikor_mod_t *monikor_mod_new(const char *name) {
  monikor_mod_t *mod;

  if (!(mod = malloc(sizeof(*mod))))
    return NULL;
  mod->name = strdup(name);
  mod->dhandle = NULL;
  mod->data = NULL;
  mod->init = NULL;
  mod->cleanup = NULL;
  mod->poll = NULL;
  return mod;
}


void monikor_mod_free(monikor_mod_t *mod) {
  if (mod) {
    free(mod->name);
    if (mod->dhandle)
      dlclose(mod->dhandle);
  }
  free(mod);
}


void monikor_mod_destroy(monikor_t *mon, monikor_mod_t *mod) {
  if (mod->cleanup)
    mod->cleanup(mon, mod->data);
  monikor_mod_free(mod);
}
