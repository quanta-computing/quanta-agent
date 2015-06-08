#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "module.h"
#include "logger.h"


static void *_load_mod_function(const char *name, const char *function, void *dl_handle) {
  char *sym_name;

  if (!(sym_name = malloc(strlen(name) + strlen(function) + 1)))
    return NULL;
  strcpy(sym_name, name);
  strcat(sym_name, function);
  return dlsym(dl_handle, sym_name);
}


static char *_get_sopath(const char *name, const char *mod_path) {
  char *sopath;
  size_t mod_path_len;

  mod_path_len = strlen(mod_path);
  if (!(sopath = malloc(mod_path_len + 1 + strlen(name) + strlen(MONIKOR_MOD_SUFFIX) + 1)))
    return NULL;
  strcpy(sopath, mod_path);
  sopath[mod_path_len] = '/';
  strcpy(sopath + mod_path_len + 1, name);
  strcat(sopath, MONIKOR_MOD_SUFFIX);
  return sopath;
}


/*
** This function will try to dynamically load a Monikor module
** It will search for the required (<mod_name>_init, <mod_name>_poll) and optional (<mod_name>_cleanup)
** symbols in an dynamic library file.
*/
monikor_mod_t *load_module(const char *name, const char *mod_path) {
  char *sopath = NULL;
  monikor_mod_t *mod = NULL;

  monikor_log(LOG_INFO, "loading module %s\n", name);
  if (!(mod = monikor_mod_new(name))
    || !(sopath = _get_sopath(name, mod_path))) {
    monikor_log(LOG_ERR, "cannot allocate memory\n");
    goto err;
  }
  if (!(mod->dhandle = dlopen(sopath, RTLD_LAZY|RTLD_LOCAL|RTLD_FIRST))) {
    monikor_log(LOG_ERR, "cannot load dynamic library %s: %s\n", sopath, dlerror());
    goto err;
  }
  if (!(mod->init = _load_mod_function(name, MONIKOR_MOD_INIT_TAG, mod->dhandle))) {
    monikor_log(LOG_ERR, "module %s does not provides function %s%s", name, name, MONIKOR_MOD_INIT_TAG);
  }
  if (!(mod->poll = _load_mod_function(name, MONIKOR_MOD_POLL_TAG, mod->dhandle))) {
    monikor_log(LOG_ERR, "module %s does not provides function %s%s", name, name, MONIKOR_MOD_POLL_TAG);
    goto err;
  }
  if (!(mod->cleanup = _load_mod_function(name, MONIKOR_MOD_CLEANUP_TAG, mod->dhandle)))
    monikor_log(LOG_INFO, "module %s does not have a %s%s function", name, name, MONIKOR_MOD_CLEANUP_TAG);
  free(sopath);
  return mod;

err:
  monikor_mod_free(mod);
  free(sopath);
  return NULL;
}
