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
  monikor_metric_store_t *metrics;
  struct timeval last_clock;
} monikor_t;

void usage(void);

int       monikor_init(monikor_t *mon, const char *config_path);
void      monikor_cleanup(monikor_t *mon);
int       monikor_run(monikor_t *mon);
int       monikor_load_all_modules(monikor_t *mon);

// Utils
char      *monikor_read_file(const char *filepath);
int       monikor_net_connect(const char *host, const char *port);
int       monikor_net_send_data(int sock, const void *data, size_t len);
int       monikor_net_send(int sock, const char *msg);
char      *monikor_net_recv(int sock);
char      *monikor_net_sr(int sock, const char *msg);
char      *monikor_net_csr(const char *host, const char *port, const char *msg);

int timecmp(const struct timeval *a, const struct timeval *b);

// DEBUG stuff
void dump_metric_list(monikor_metric_list_t *list);

#endif /* end of include guard: MONIKOR_H_ */
