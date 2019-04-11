#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <inttypes.h>

#include "monikor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <postgresql/libpq-fe.h>

#include "postgresql.h"

int max_connection(monikor_t *mon, postgresql_module_t *mod,
struct timeval *clock, PGresult *result) {
  char metric_name[1024];
  char *value;

  sprintf(metric_name, "postgresql.connections.max.%s", mod->database);
  if (!(value = PQgetvalue(result, 0, 1)))
    return 0;
  return monikor_metric_push(mon, monikor_metric_integer(
    metric_name, clock, strtoull(value, NULL, 0), 0));
}

int database_size(monikor_t *mon, postgresql_module_t *mod,
struct timeval *clock, PGresult *result) {
  char metric_name[1024];
  char *value;

  // Query return databse_size as a string stocked at 0/0 coordinates in result
  // Result seems to be in bytes
  if (!(value = PQgetvalue(result, 0, 0)))
    return 0;
  sprintf(metric_name, "postgresql.database_size.%s", mod->database);
  return monikor_metric_push(mon, monikor_metric_integer(
    metric_name, clock, strtoull(value, NULL, 0), 0));
}

int pg_locks(monikor_t *mon, postgresql_module_t *mod, struct timeval *clock, PGresult *result) {
  char metric_name[1024];
  char *value;

  sprintf(metric_name, "postgresql.locks.%s", mod->database);
  // This table stock lock information, probably needs to be parsed by locktype
  if (!(value = PQgetvalue(result, 0, 0)))
    return 0;
  return monikor_metric_push(mon, monikor_metric_integer(
    metric_name, clock, strtoull(value, NULL, 10), 0));
}

int stat_activity(monikor_t *mon, postgresql_module_t *mod,
struct timeval *clock, PGresult *result) {
  char metric_name[1024];

  sprintf(metric_name, "postgresql.connections.active.%s", mod->database);
  // This table stock user-related informations, I believe each row represent an User, need confirmation
  return monikor_metric_push(mon, monikor_metric_integer(
    metric_name, clock, PQntuples(result), 0));
}

int stat_database(monikor_t *mon, postgresql_module_t *mod,
struct timeval *clock, PGresult *result) {
  int n = 0;
  char metric_name[1024];
  char *value;

  // Postgresql transactions
  sprintf(metric_name, "postgresql.transactions.commits.%s", mod->database);
  if ((value = PQgetvalue(result, 0, 3)))
    n += monikor_metric_push(mon, monikor_metric_integer(metric_name,
      clock, strtoull(value, NULL, 0), MONIKOR_METRIC_TIMEDELTA));

  sprintf(metric_name, "postgresql.transactions.rollbacks.%s", mod->database);
  if ((value = PQgetvalue(result, 0, 4)))
    n += monikor_metric_push(mon, monikor_metric_integer(metric_name,
      clock, strtoull(value, NULL, 0), MONIKOR_METRIC_TIMEDELTA));

  // Postgresql queries
  sprintf(metric_name, "postgresql.rows.returned.%s", mod->database);
  if ((value = PQgetvalue(result, 0, 7)))
    n += monikor_metric_push(mon, monikor_metric_integer(metric_name,
      clock, strtoull(value, NULL, 0), MONIKOR_METRIC_TIMEDELTA));

  sprintf(metric_name, "postgresql.rows.fetched.%s", mod->database);
  if ((value = PQgetvalue(result, 0, 8)))
    n += monikor_metric_push(mon, monikor_metric_integer(metric_name,
      clock, strtoull(value, NULL, 0), MONIKOR_METRIC_TIMEDELTA));

  sprintf(metric_name, "postgresql.rows.inserted.%s", mod->database);
  if ((value = PQgetvalue(result, 0, 9)))
    n += monikor_metric_push(mon, monikor_metric_integer(metric_name,
      clock, strtoull(value, NULL, 0), MONIKOR_METRIC_TIMEDELTA));

  sprintf(metric_name, "postgresql.rows.updated.%s", mod->database);
  if ((value = PQgetvalue(result, 0, 10)))
    n += monikor_metric_push(mon, monikor_metric_integer(metric_name,
      clock, strtoull(value, NULL, 0), MONIKOR_METRIC_TIMEDELTA));

  sprintf(metric_name, "postgresql.rows.deleted.%s", mod->database);
  if ((value = PQgetvalue(result, 0, 11)))
    n += monikor_metric_push(mon, monikor_metric_integer(metric_name,
      clock, strtoull(value, NULL, 0), MONIKOR_METRIC_TIMEDELTA));

  // Postgresql deadlocks
  sprintf(metric_name, "postgresql.deadlocks_per_second.%s", mod->database);
  if ((value = PQgetvalue(result, 0, 15)))
    n += monikor_metric_push(mon, monikor_metric_integer(metric_name,
      clock, strtoull(value, NULL, 0), MONIKOR_METRIC_TIMEDELTA));
  return n;
}

typedef int (*poll_func_t)(monikor_t *, postgresql_module_t *, struct timeval *, PGresult *);
static const poll_func_t pollers[] = {
  stat_activity,
  stat_database,
  database_size,
  pg_locks,
  max_connection,
  NULL
};

static const char *sql_query_format = "SELECT * FROM pg_stat_activity WHERE datname = %s;"
  "SELECT * FROM pg_stat_database WHERE datname = %s;"
  "SELECT pg_database_size(%s);"
  "SELECT COUNT(1) FROM pg_locks AS l INNER JOIN pg_class AS pc ON (l.relation = pc.oid) WHERE relname NOT LIKE 'pg\\_%%' AND l.mode IS NOT NULL;"
  "SELECT * FROM pg_settings WHERE name = 'max_connections';";

static const char *connect_fmt =
  "host=%s port=%s dbname=postgres user=%s password=%s connect_timeout=2";

int postgresql_poll_metrics(monikor_t *mon, struct timeval *clock, postgresql_module_t *mod) {
  int n = 0;
  PGconn *dbh = NULL;
  PGresult *result;
  char *sql_query = NULL;
  char *escaped_db_name = NULL;
  char *connect_opts = NULL;

  if (asprintf(&connect_opts, connect_fmt, mod->host, mod->port, mod->user, mod->password) <= 0)
    goto end;
  dbh = PQconnectdb(connect_opts);
  if (PQstatus(dbh) != CONNECTION_OK) {
    monikor_log_mod(LOG_ERR, MOD_NAME,
      "cannot connect to postgresql: %s", PQerrorMessage(dbh));
    goto end;
  }
  if (!(escaped_db_name = PQescapeLiteral(dbh, mod->database, strlen(mod->database)))
  || asprintf(&sql_query, sql_query_format, escaped_db_name, escaped_db_name, escaped_db_name) <= 0) {
    monikor_log_mod(LOG_ERR, MOD_NAME, "cannot prepare SQL query\n");
    goto end;
  }
  if (!PQsendQuery(dbh, sql_query)) {
    monikor_log_mod(LOG_ERR, MOD_NAME,
      "cannot execute postgresql query: %s", PQerrorMessage(dbh));
    goto end;
  }

  for (size_t i = 0; pollers[i]; i++) {
    result = PQgetResult(dbh);
    if (!result)
      continue;
    if (PQresultStatus(result) == PGRES_TUPLES_OK) {
      n += pollers[i](mon, mod, clock, result);
    } else {
      monikor_log_mod(LOG_ERR, MOD_NAME, "bad result: %s\n",
        PQresStatus(PQresultStatus(result)));
    }
    PQclear(result);
  }

end:
  PQfreemem(escaped_db_name);
  free(sql_query);
  free(connect_opts);
  PQfinish(dbh);
  return n;
}
