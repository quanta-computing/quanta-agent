#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <inttypes.h>

#include "monikor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <libpq-fe.h>

int max_connection(monikor_t mon, struct timeval *clock, PGresult *result)
{
  return monikor_metric_push(mon, monikor_metric_integer(
    "postgresql.max_connections", clock, strtoull(PQgetvalue(result, 0, 1), NULL, 0), 0));
}

int database_size(monikor_t mon, struct timeval *clock, PGresult *result)
{
  // Query return databse_size as a string stocked at 0/0 coordinates in result
  // Result seems to be in bytes
  return monikor_metric_push(mon, monikor_metric_integer(
    "postgresql.database_size", clock, strtoull(PQgetvalue(result, 0, 0), NULL, 0), 0));
}

int pg_locks(monikor_t mon, struct timeval *clock, PGresult *result)
{
  // This table stock lock information, probably needs to be parsed by locktype
  return monikor_metric_push(mon, monikor_metric_integer(
    "locks", clock, PQntuples(result), 0));
}

int stat_activity(monikor_t mon, struct timeval *clock, PGresult *result)
{
  // This table stock user-related informations, I believe each row represent an User, need confirmation
  return monikor_metric_push(mon, monikor_metric_integer(
    "active.connections", clock, PQntuples(result), 0));
}

int stat_database(monikor_t mon, struct timeval *clock, PGresult *result)
{
  int n = 0;

  // Postgresql transactions
  n += monikor_metric_push(mon, monikor_metric_integer("commits/s",
    clock, strtoull(PQgetvalue(result, 0, 3), NULL, 0), MONIKOR_METRIC_TIMEDELTA));

  n += monikor_metric_push(mon, monikor_metric_integer("rollback/s",
    clock, strtoull(PQgetvalue(result, 0, 4), NULL, 0), MONIKOR_METRIC_TIMEDELTA));

  // Postgresql queries
  n += monikor_metric_push(mon, monikor_metric_integer("postgresql.rows_returned",
    clock, strtoull(PQgetvalue(result, 0, 7), NULL, 0), MONIKOR_METRIC_TIMEDELTA));

  n += monikor_metric_push(mon, monikor_metric_integer("postgresql.rows_fetched",
    clock, strtoull(PQgetvalue(result, 0, 8), NULL, 0), MONIKOR_METRIC_TIMEDELTA));

  n += monikor_metric_push(mon, monikor_metric_integer("postgresql.rows_inserted",
    clock, strtoull(PQgetvalue(result, 0, 9), NULL, 0), MONIKOR_METRIC_TIMEDELTA));

  n += monikor_metric_push(mon, monikor_metric_integer("postgresql.rows_updated",
    clock, strtoull(PQgetvalue(result, 0, 10), NULL, 0), MONIKOR_METRIC_TIMEDELTA));

  n += monikor_metric_push(mon, monikor_metric_integer("postgresql.rows_deleted",
    clock, strtoull(PQgetvalue(result, 0, 11), NULL, 0), MONIKOR_METRIC_TIMEDELTA));

    // Postgresql locks
  n += monikor_metric_push(mon, monikor_metric_integer("deadlocks/s",
    clock, strtoull(PQgetvalue(result, 0, 15), NULL, 0), MONIKOR_METRIC_TIMEDELTA));
  return n;
}

int postgresql_poll_metrics(monikor_t *mon, struct timeval *clock))
{
  uint64_t value;
  int n = 0;
  PGconn *connectdb;
  PGresult *result;
  ExecStatusType status;

  // Connecting to database and checking for errors
  connectdb = PQsetdb("127.0.0.1", NULL, NULL, NULL, "postgres");
  if (PQstatus(connectdb) != CONNECTION_OK)
    return 0;

  //First query : pg_stat_activity
  result = PQexec(connectdb, "select * from pg_stat_activity");
  status = PQresultStatus(result); // Check for memory leaks
  if (strcmp(PQresStatus(status), "PGRES_TUPLES_OK") == 0)
    n += stat_activity(mon, clock, result);
  PQclear(result);

  //Second query : pg_stat_database
  result = PQexec(connectdb, "select * from pg_stat_database");
  status = PQresultStatus(result); // Check for memory leaks
  if (strcmp(PQresStatus(status), "PGRES_TUPLES_OK") == 0)
    n += stat_database(mon, clock, result);
  PQclear(result);

  //Third query : pg_locks
  result = PQexec(connectdb, "select * from pg_locks");
  status = PQresultStatus(result); // Check for memory leaks
  if (strcmp(PQresStatus(status), "PGRES_TUPLES_OK") == 0)
    n += pg_locks(mon, clock, result);
  PQclear(result);

  //Fourth query : pg_database_size
  result = PQexec(connectdb, "select pg_database_size('postgres');");
  status = PQresultStatus(result);
  if (strcmp(PQresStatus(status), "PGRES_TUPLES_OK") == 0)
    n += database_size(mon, clock, result);
  PQclear(result);

  //Fifth query : max connection
  result = PQexec(connectdb, "select * from pg_settings where name = 'max_connections';");
  status = PQresultStatus(result);
  if (strcmp(PQresStatus(status), "PGRES_TUPLES_OK") == 0)
    n += max_connection(mon, clock, result);
  PQclear(result);

  //free memory
  PQfinish(connectdb);
  return n;
}
