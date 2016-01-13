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
  return 0;
}
