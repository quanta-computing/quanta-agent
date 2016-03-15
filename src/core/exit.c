#include <stdlib.h>
#include <unistd.h>

#include "monikor.h"

void monikor_exit(monikor_t *mon) {
  monikor_log(LOG_ERR, "Exiting...\n");
  unlink(mon->config->pidfile);
  monikor_cleanup(mon);
  exit(EXIT_SUCCESS);
}
