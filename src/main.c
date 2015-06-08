#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "monikor.h"

void dump_metric_list(monikor_metric_list_t *list);


int main(int argc, char **argv) {
  monikor_mod_t *mod;
  monikor_metric_list_t *metrics;
  monikor_config_t *config;
  int i;

  if (argc != 3)
    return 1;
  monikor_logger_init(LOG_DEBUG);
  if (!(config = monikor_load_config(argv[2])))
    return 1;
  dump_config(config->full_config);
  if (!(mod = load_module(argv[1], "./lib/modules")))
    return 1;
  mod->init();
  for (i = 0; i < 3; i++) {
    metrics = mod->poll();
    if (metrics)
      dump_metric_list(metrics);
    monikor_metric_list_free(metrics);
    sleep(1);
  }
  monikor_mod_destroy(mod);
  return 0;
}
