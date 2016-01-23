#include <string.h>
#include <stdlib.h>

#include <mysql/mysql.h>

#include "monikor.h"
#include "mysqlstat.h"

static const struct {
  char *field;
  char *name;
  uint8_t flags;
} metrics[] = {
  {"Bytes_received", "rx.bytes", MONIKOR_METRIC_TIMEDELTA},
  {"Bytes_sent", "tx.bytes", MONIKOR_METRIC_TIMEDELTA},
  {"Com_delete", "queries.delete", MONIKOR_METRIC_TIMEDELTA},
  {"Com_insert", "queries.insert", MONIKOR_METRIC_TIMEDELTA},
  {"Com_select", "queries.select", MONIKOR_METRIC_TIMEDELTA},
  {"Com_update", "queries.update", MONIKOR_METRIC_TIMEDELTA},
  {"Threads_connected", "connections.current", 0},
  {"Threads_running", "connections.active", 0},
  {"max_connections", "connections.max", 0},
  {NULL, NULL, 0}
};

static MYSQL *_mysql_connect(monikor_mysql_config_t *config) {
  MYSQL *dbh;
  MYSQL *err;

  if (!(dbh = mysql_init(NULL))) {
    monikor_log_mod(LOG_ERR, MOD_NAME, "Cannot allocate memory\n");
    return NULL;
  }
  err = mysql_real_connect(dbh, config->host, config->user, config->password, NULL, config->port,
    NULL, CLIENT_MULTI_STATEMENTS);
  if (!err) {
    monikor_log_mod(LOG_ERR, MOD_NAME, "Cannot connect to MySQL: %s\n", mysql_error(dbh));
    mysql_close(dbh);
    return NULL;
  }
  return dbh;
}

int fetch_mysql_metrics(monikor_t *mon, struct timeval *clock, MYSQL_RES *result) {
  int n = 0;
  char metric_name[256];
  MYSQL_ROW row;

  while ((row = mysql_fetch_row(result))) {
    for (size_t i = 0; metrics[i].field; i++) {
      if (!strcmp(row[0], metrics[i].field)) {
        sprintf(metric_name, "mysql.%s", metrics[i].name);
        monikor_metric_push(mon, monikor_metric_integer(
          metric_name, clock, (uint64_t)strtoull(row[1], NULL, 10), metrics[i].flags));
        n++;
      }
    }
  }
  return n;
}

int mysqlstat_poll_metrics(monikor_t *mon, struct timeval *clock, monikor_mysql_config_t *config) {
  MYSQL *dbh;
  MYSQL_RES *result;
  int err;
  int n = 0;

  if (!(dbh = _mysql_connect(config)))
    return -1;
  if (mysql_query(dbh, "SHOW GLOBAL STATUS;SHOW VARIABLES")) {
    monikor_log_mod(LOG_ERR, MOD_NAME, "MySQL query error: %s\n", mysql_error(dbh));
    mysql_close(dbh);
    return -1;
  }
  do {
    if (!(result = mysql_store_result(dbh))) {
      err = 1;
      break;
    }
    n += fetch_mysql_metrics(mon, clock, result);
    mysql_free_result(result);
    err = mysql_next_result(dbh);
  } while (!err);
  if (err > 0) {
    monikor_log_mod(LOG_ERR, MOD_NAME, "MySQL error: %s\n", mysql_error(dbh));
    mysql_close(dbh);
    return -1;
  }
  mysql_close(dbh);
  return n;
}
