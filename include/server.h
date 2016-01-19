#ifndef MONIKOR_SERVER_H_
#define MONIKOR_SERVER_H_

#include <stdint.h>
#include "metric.h"
#include "io_handler.h"

#ifndef MONIKOR_STRUCT_TYPEDEF_DECL
#define MONIKOR_STRUCT_TYPEDEF_DECL
typedef struct monikor_s monikor_t;
#endif

typedef struct {
  int socket;
  monikor_serialized_metric_list_hdr_t header;
  void *data;
} monikor_client_t;

#define MONIKOR_CLIENT_INACTIVE_SOCKET -1
#define MONIKOR_SRV_MAX_CLIENTS 10

typedef struct {
  int socket;
  size_t n_clients;
  monikor_client_t clients[MONIKOR_SRV_MAX_CLIENTS];
  monikor_t *mon;
  monikor_io_handler_t *handler;
} monikor_server_t;

#include "monikor.h"
#include "io_handler.h"

typedef struct {
  monikor_client_t *client;
  monikor_server_t *server;
  monikor_io_handler_t *handler;
} monikor_server_handler_t;

monikor_io_handler_t *monikor_server_handler_new(monikor_server_t *server, monikor_client_t *client);
void monikor_server_handler_free(monikor_io_handler_t *handler);

void monikor_client_init(monikor_client_t *client);
int monikor_server_init(monikor_server_t *server, monikor_t *mon);
void monikor_server_cleanup(monikor_server_t *server);
int monikor_server_handle_connection(monikor_server_t *server);
int monikor_server_handle_client(monikor_server_handler_t *handler);
void monikor_server_handle_any(monikor_io_handler_t *handler, uint8_t mode);
void monikor_server_disconnect(monikor_server_handler_t *handler);

#endif /* end of include guard: MONIKOR_SERVER_H_ */
