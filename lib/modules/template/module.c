#include <stdlib.h>
#include <time.h>
#include "monikor.h"

#define MOD_NAME "__NAME__"

void __NAME___init(void) {
}


monikor_metric_list_t *__NAME___poll(void) {
  monikor_metric_list_t *metrics;
  time_t now = time(NULL);

  if (!(metrics = monikor_metric_list_new()))
    return NULL;
  return metrics;
}


void __NAME___cleanup(void) {
}
