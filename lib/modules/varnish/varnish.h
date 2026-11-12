#ifndef varnish_H_
#define varnish_H_

#include "monikor.h"

#define MOD_NAME "varnish"

#define MONIKOR_VARNISH_DEFAULT_VARNISHSTAT_PATH "varnishstat"
#define MONIKOR_VARNISH_DEFAULT_VARNISHSTAT_TIMEOUT_ARG "10"
#define MONIKOR_VARNISH_VARNISHSTAT_MAX_ARGS 6
#define MONIKOR_VARNISH_MAX_INSTANCE_LENGTH 128


typedef struct {
  char *instance;
  char *varnishstat_path;
  char *varnishstat_timeout_arg;
  monikor_t *mon;
  monikor_process_handler_t *cmd_handler;
} varnish_module_t;

size_t varnish_fetch_metrics(varnish_module_t *mod, const char *output);
void varnish_poll_metrics(command_exec_t *cmd);

#endif /* end of include guard: varnish_H_ */
