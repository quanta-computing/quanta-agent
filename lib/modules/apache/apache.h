#ifndef APACHE_H_
#define APACHE_H_

#include "monikor.h"
#define MOD_NAME "apache"

#define APACHE_HOST "localhost"
#define APACHE_PORT "80"
#define APACHE_SERVER_URL "/server-status?auto"

int apache_poll_metrics(monikor_metric_list_t *metrics, time_t clock);


#endif /* end of include guard: APACHE_H_ */
