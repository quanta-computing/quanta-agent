#include <stdlib.h>
#include <time.h>

#include "monikor.h"
#include "varnish.h"


void *varnish_init(monikor_t *mon, monikor_config_dict_t *config) {
  struct VSM_data *vd;

  (void)mon;
  (void)config;
  vd = VSM_New();
  VSC_Setup(vd);
  return (void *)vd;
}

void varnish_cleanup(monikor_t *mon, void *data) {
  (void)mon;
  VSM_Delete((struct VSM_data *)data);
}

int varnish_poll(monikor_t *mon, void *data) {
  struct timeval now;

  gettimeofday(&now, NULL);
  return varnish_poll_metrics(mon, &now, (struct VSM_data *)data);
}
