#ifndef APACHE_H_
#define APACHE_H_

#include "monikor.h"
#define MOD_NAME "apache"

#define APACHE_DEFAULT_STATUS_URL "http://localhost/server-status?auto"
#define APACHE_TIMEOUT 5

int apache_poll_metrics(monikor_t *mon, const char *url);


#endif /* end of include guard: APACHE_H_ */
