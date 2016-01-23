#ifndef memcached_H_
#define memcached_H_

#include "monikor.h"

#define MOD_NAME "memcached"
#define MONIKOR_MEMCACHED_DEFAULT_HOST "localhost"
#define MONIKOR_MEMCACHED_DEFAULT_PORT "11211"


typedef struct {
  char *host;
  char *port;
  monikor_t *mon;
  monikor_io_handler_t *io_handler;
} memcached_module_t;

void memcached_poll_metrics(char *status, void *data);


#endif /* end of include guard: memcached_H_ */
