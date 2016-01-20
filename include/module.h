#ifndef MONIKOR_MODULE_H_
# define MONIKOR_MODULE_H_

# include "metric.h"

#ifndef MONIKOR_STRUCT_TYPEDEF_DECL
#define MONIKOR_STRUCT_TYPEDEF_DECL
typedef struct monikor_s monikor_t;
#endif

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
  void    *data;
  void    *(*init)(monikor_t *mon);
  void    (*cleanup)(monikor_t *mon, void *data);
  int     (*poll)(monikor_t *mon, void *data);
} monikor_mod_t;


monikor_mod_t       *monikor_load_module(const char *name, const char *mod_path);

monikor_mod_t       *monikor_mod_new(const char *name);
void                monikor_mod_free(monikor_mod_t *mod);
void                monikor_mod_destroy(monikor_t *mon, monikor_mod_t *mod);


#endif /* end of include guard: MONIKOR_MODULE_H_ */
