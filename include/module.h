#ifndef MONIKOR_MODULE_H_
# define MONIKOR_MODULE_H_

# include "monikor.h"

# define MONIKOR_MOD_INIT_TAG "_init"
# define MONIKOR_MOD_POLL_TAG "_poll"
# define MONIKOR_MOD_CLEANUP_TAG "_cleanup"

# ifdef __APPLE__
#  define MONIKOR_MOD_SUFFIX ".dylib"
# else
#  define MONIKOR_MOD_SUFFIX ".so"
# endif

typedef struct {
  char    *name;
  void    *dhandle;
  void    (*init)(void);
  void    (*cleanup)(void);
  monikor_metric_list_t*    (*poll)(void);

} monikor_mod_t;


monikor_mod_t       *load_module(const char *name, const char *mod_path);

monikor_mod_t       *monikor_mod_new(const char *name);
void                monikor_mod_free(monikor_mod_t *mod);
void                monikor_mod_destroy(monikor_mod_t *mod);


#endif /* end of include guard: MONIKOR_MODULE_H_ */
