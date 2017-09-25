#ifndef postgresql_H_
#define postgresql_H_

#include "monikor.h"

#define MOD_NAME "postgresql"
#define POSTGRESQL_DEFAULT_HOST "127.0.0.1"
#define POSTGRESQL_DEFAULT_PORT "5432"
#define MONIKOR_POSTGRES_MAX_INSTANCE_LENGTH 64

typedef struct {
  char *host;
  char *port;
  char *user;
  char *password;
  char *database;
} postgresql_module_t;

int postgresql_poll_metrics(monikor_t *mon, struct timeval *clock, postgresql_module_t *mod);

#endif /* end of include guard: postgresql_H_ */
