#ifndef MONIKOR_H_
# define MONIKOR_H_

# include <stddef.h>
# include <stdint.h>
# include <sys/time.h>


#ifndef MONIKOR_STRUCT_TYPEDEF_DECL
#define MONIKOR_STRUCT_TYPEDEF_DECL
typedef struct monikor_s monikor_t;
#endif

# include "server.h"
# include "strl.h"
# include "metric.h"
# include "module.h"
# include "logger.h"
# include "config.h"
# include "utils.h"
# include "io_handler.h"

struct monikor_s {
  monikor_config_t *config;
  struct {
    monikor_mod_t **modules;
    size_t count;
  } modules;
  monikor_metric_store_t *metrics;
  monikor_server_t server;
  monikor_io_handler_list_t io_handlers;
  struct timeval last_clock;
};

// misc
void usage(void);

// core
int       monikor_init(monikor_t *mon, const char *config_path);
void      monikor_cleanup(monikor_t *mon);
int       monikor_run(monikor_t *mon);
int       monikor_load_all_modules(monikor_t *mon);
int       monikor_send_metrics(monikor_t *mon);

// IO handling
void monikor_register_io_handler(monikor_t *mon, monikor_io_handler_t *handler);
void monikor_unregister_io_handler(monikor_t *mon, monikor_io_handler_t *handler);

#endif /* end of include guard: MONIKOR_H_ */
