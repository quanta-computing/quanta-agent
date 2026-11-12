#ifndef mysql_H_
#define mysql_H_

#include "monikor.h"

#define MOD_NAME "mysqlstat"
#define MONIKOR_MYSQL_DEFAULT_HOST "localhost"
#define MONIKOR_MYSQL_TIMEOUT 2

typedef struct {
  char *host;
  int port;
  char *user;
  char *password;
  unsigned int timeout;
} monikor_mysql_config_t;


int mysqlstat_poll_metrics(monikor_t *mon, struct timeval *clock, monikor_mysql_config_t *config);

#endif /* end of include guard: mysql_H_ */
