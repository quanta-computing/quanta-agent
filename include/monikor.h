#ifndef MONIKOR_H_
# define MONIKOR_H_

# include <stddef.h>
# include <sys/time.h>

# include "strl.h"
# include "metric.h"
# include "module.h"
# include "logger.h"
# include "config.h"

typedef struct {
  monikor_config_t *config;

  struct {
    monikor_mod_t **modules;
    size_t count;
  } modules;

  struct timeval last_clock;
} monikor_t;

void usage(void);

int       monikor_init(monikor_t *mon, const char *config_path);
void      monikor_cleanup(monikor_t *mon);
int       monikor_run(monikor_t *mon);
int       monikor_load_all_modules(monikor_t *mon);

// DEBUG stuff
void dump_metric_list(monikor_metric_list_t *list);

#endif /* end of include guard: MONIKOR_H_ */
