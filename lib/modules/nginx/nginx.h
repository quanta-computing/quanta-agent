#ifndef nginx_H_
#define nginx_H_

#include "monikor.h"

#define MOD_NAME "nginx"

#define NGINX_DEFAULT_STATUS_URL "http://localhost/nginx-status"
#define NGINX_TIMEOUT 3

int nginx_poll_metrics(monikor_t *mon, struct timeval *clock, const char *url);

#endif /* end of include guard: nginx_H_ */
