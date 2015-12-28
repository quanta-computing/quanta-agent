#include <stdlib.h>
#include <time.h>

#include "monikor.h"
#include "__NAME__.h"


void __NAME___init(void) {
}

void __NAME___cleanup(void) {
}

monikor_metric_list_t *__NAME___poll(void) {
  monikor_metric_list_t *metrics;
  time_t now = time(NULL);

  if (!(metrics = monikor_metric_list_new()))
    return NULL;
  return metrics;
}
