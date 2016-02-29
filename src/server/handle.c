#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "monikor.h"
#include "metric.h"
#include "server.h"


int monikor_server_handle_connection(monikor_server_t *server) {
  monikor_io_handler_t *handler;

  if (server->n_clients >= MONIKOR_SRV_MAX_CLIENTS)
    return -1;
  for (size_t i = 0; i < MONIKOR_SRV_MAX_CLIENTS; i++) {
    if (server->clients[i].socket == MONIKOR_CLIENT_INACTIVE_SOCKET) {
      struct sockaddr_un addr;
      socklen_t addrlen = sizeof(addr);

      if ((server->clients[i].socket = accept(server->socket, (struct sockaddr *)&addr, &addrlen)) == -1)
        return -1;
      if (!(handler = monikor_server_handler_new(server, &server->clients[i]))) {
        if (server->clients[i].socket != -1)
          close(server->clients[i].socket);
        monikor_client_init(&server->clients[i]);
        return -1;
      }
      monikor_register_io_handler(server->mon, handler);
      server->n_clients++;
      monikor_log(LOG_INFO, "Client connected on socket %d\n", server->clients[i].socket);
      return 0;
    }
  }
  return 1;
}

int monikor_server_handle_client(monikor_server_handler_t *handler) {
  char buf[SERIALIZED_METRIC_LIST_HDR_SIZE];
  ssize_t ret;
  size_t rd;
  monikor_metric_list_t *metrics;

  monikor_log(LOG_DEBUG, "Receiving data from client on socket %d\n", handler->client->socket);
  ret = read(handler->client->socket, buf, SERIALIZED_METRIC_LIST_HDR_SIZE);
  if (ret < (ssize_t)SERIALIZED_METRIC_LIST_HDR_SIZE)
    goto err;
  monikor_metric_list_header_unserialize((void *)buf, &handler->client->header);
  if (!(handler->client->data = malloc(handler->client->header.data_size))) {
    monikor_log(LOG_ERR, "cannot allocate %zu bytes of memory\n", handler->client->header.data_size);
    goto err;
  }
  rd = 0;
  do {
    ret = read(handler->client->socket, handler->client->data + rd, handler->client->header.data_size - rd);
    if (ret == -1) {
      monikor_log(LOG_ERR, "read(2) error: %s\n", strerror(errno));
      goto err;
    }
    rd += ret;
  } while (rd < handler->client->header.data_size);
  if (rd < handler->client->header.data_size)
    goto err;
  monikor_metric_list_unserialize(handler->client->data, &handler->client->header, &metrics);
  monikor_metric_store_lpush(handler->server->mon->metrics, metrics);
  monikor_metric_list_free(metrics);
  monikor_server_disconnect(handler);
  return 0;

err:
  monikor_log(LOG_WARNING, "Protocol error from client %d\n", handler->client->socket);
  handler->client->header.count = 0;
  monikor_server_disconnect(handler);
  return -1;
}

void monikor_server_handle_any(monikor_io_handler_t *handler, uint8_t mode) {
  monikor_server_handler_t *srv_handler = (monikor_server_handler_t *)handler->data;

  (void)mode;
  if (srv_handler->client)
    monikor_server_handle_client(srv_handler);
  else
    monikor_server_handle_connection(srv_handler->server);
}
