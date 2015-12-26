#include <stdio.h>
#include <stdlib.h>

#include "monikor.h"

int main(int argc, char **argv) {
  monikor_t mon;

  if (argc != 2)
    usage();
  if (monikor_init(&mon, argv[1]))
    return 1;
  monikor_run(&mon);
  monikor_logger_cleanup();
  // if (!(mod = load_module(argv[1], "./lib/modules")))
  //   return 1;
  // mod->init();
  // for (i = 0; i < 3; i++) {
  //   metrics = mod->poll();
  //   if (metrics)
  //     dump_metric_list(metrics);
  //   monikor_metric_list_free(metrics);
  //   sleep(1);
  // }
  // monikor_mod_destroy(mod);
  return 0;
}
