#ifndef nginx_H_
#define nginx_H_

#include "monikor.h"

#define MOD_NAME "nginx"

#define NGINX_DEFAULT_STATUS_URL "http://localhost/nginx-status"
#define NGINX_TIMEOUT 3

void nginx_poll_metrics(http_response_t *status, CURLcode result);

#endif /* end of include guard: nginx_H_ */
