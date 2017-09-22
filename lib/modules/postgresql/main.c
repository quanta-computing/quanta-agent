#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <inttypes.h>

// #include "monikor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <libpq-fe.h>

char *get_commit(PGconn *connectdb, PGresult *result)
{
  return PQgetvalue(result, 0, 3);
}

char *get_rollback(PGconn *connectdb, PGresult *result)
{
  return PQgetvalue(result, 0, 4);
}

char *get_returned(PGconn *connectdb, PGresult *result)
{
  return PQgetvalue(result, 0, 7);
}

char *get_fetched(PGconn *connectdb, PGresult *result)
{
  return PQgetvalue(result, 0, 8);
}

char *get_inserted(PGconn *connectdb, PGresult *result)
{
  return PQgetvalue(result, 0, 9);
}

char *get_updated(PGconn *connectdb, PGresult *result)
{
  return PQgetvalue(result, 0, 10);
}

char *get_deleted(PGconn *connectdb, PGresult *result)
{
  return PQgetvalue(result, 0, 11);
}

char *get_deadlock(PGconn *connectdb, PGresult *result)
{
  return PQgetvalue(result, 0, 15);
}

int main()
{
  //Connect
  PGconn *connectdb;
  PQconninfoOption *parse;

  //Query
  PGresult *result;
  ExecStatusType status;

  // Connecting to postgres database
  connectdb = PQsetdb("127.0.0.1", NULL, NULL, NULL, "postgres");
  if (PQstatus(connectdb) != CONNECTION_OK)
  {
    printf("Connection to database failed: %s", PQerrorMessage(connectdb));
    return 0;
  }

  // Making query
  result = PQexec(connectdb, "select * from pg_stat_activity");
  status = PQresultStatus(result);
  //Check query status
  if (strcmp(PQresStatus(status), "PGRES_TUPLES_OK") != 0)
  {
    printf("Query resStatus = %s\n", PQresStatus(status));
    printf("Query error = %s\n", PQresultErrorMessage(result));
  }
  printf("Number of active connections = %d\n", PQntuples(result));
  PQclear(result);

//******************** pg_stat_database **************************
  result = PQexec(connectdb, "select * from pg_stat_database");
  status = PQresultStatus(result);
  //Check query status
  if (strcmp(PQresStatus(status), "PGRES_TUPLES_OK") != 0)
  {
    printf("Query resStatus = %s\n", PQresStatus(status));
    printf("Query error = %s\n", PQresultErrorMessage(result));
  }
  printf("Number of transactions in this database that have been committed = %s\n",get_commit(connectdb, result));
  printf("Number of transactions in this database that have been rolled back = %s\n",get_rollback(connectdb, result));
  printf("Number of rows returned by queries in this database = %s\n",get_returned(connectdb, result));
  printf("Number of rows fetched by queries in this database = %s\n",get_fetched(connectdb, result));
  printf("Number of rows inserted by queries in this database = %s\n",get_inserted(connectdb, result));
  printf("Number of rows updated by queries in this database = %s\n",get_updated(connectdb, result));
  printf("Number of rows deleted by queries in this database = %s\n",get_deleted(connectdb, result));
  printf("Number of deadlocks detected in this database = %s\n",get_deadlock(connectdb, result));

  // Database size
  result = PQexec(connectdb, "select pg_database_size('postgres');");
  status = PQresultStatus(result);
  if (strcmp(PQresStatus(status), "PGRES_TUPLES_OK") != 0)
  {
    printf("Database size Query resStatus = %s\n", PQresStatus(status));
    printf("Database size Query error = %s\n", PQresultErrorMessage(result));
  }
  printf("Database_size = %s\n", PQgetvalue(result, 0, 0));

  // Max connection
  result = PQexec(connectdb, "select * from pg_settings where name = 'max_connections';");
  status = PQresultStatus(result);
  if (strcmp(PQresStatus(status), "PGRES_TUPLES_OK") != 0)
  {
    printf("Max connection Query resStatus = %s\n", PQresStatus(status));
    printf("Max connection Query error = %s\n", PQresultErrorMessage(result));
  }
  printf("Max connections = %s\n", PQgetvalue(result, 0, 1));

  PQclear(result);
  PQfinish(connectdb);
  return 0;
}
