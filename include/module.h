#ifndef MONIKOR_MODULE_H_
# define MONIKOR_MODULE_H_

# include <sys/time.h>

# include "metric.h"
# include "config.h"

# ifndef MONIKOR_STRUCT_TYPEDEF_DECL
#  define MONIKOR_STRUCT_TYPEDEF_DECL
typedef struct monikor_s monikor_t;
# endif

# define MONIKOR_MOD_INIT_TAG "_init"
# define MONIKOR_MOD_POLL_TAG "_poll"
# define MONIKOR_MOD_CLEANUP_TAG "_cleanup"

# ifdef __APPLE__
#  define MONIKOR_MOD_SUFFIX ".dylib"
# else
#  define MONIKOR_MOD_SUFFIX ".so"
# endif

typedef struct {
  char *name;
  void *data;
  void *dhandle;
  int poll_interval;
  struct timeval last_clock;
  monikor_config_dict_t *config;
  void *(*init)(monikor_t *mon, monikor_config_dict_t *cfg);
  void (*cleanup)(monikor_t *mon, void *data);
  int (*poll)(monikor_t *mon, void *data);
} monikor_mod_t;


monikor_mod_t       *monikor_load_module(const char *name, const char *mod_path);

monikor_mod_t       *monikor_mod_new(const char *name);
void                monikor_mod_free(monikor_mod_t *mod);
void                monikor_mod_destroy(monikor_t *mon, monikor_mod_t *mod);

int monikor_info_module_poll(monikor_t *mon, void *data);
int monikor_info_module_load(monikor_t *mon);

#endif /* end of include guard: MONIKOR_MODULE_H_ */
