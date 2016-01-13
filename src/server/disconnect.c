#include <stdlib.h>
#include <unistd.h>

#include "server.h"
#include "monikor.h"

void monikor_server_disconnect(monikor_server_handler_t *handler) {
  monikor_log(LOG_INFO, "Disconnecting client from socket %d (got %zu metrics)\n",
    handler->client->socket, handler->client->header.count
  );
  close(handler->client->socket);
  free(handler->client->data);
  monikor_client_init(handler->client);
  handler->server->n_clients--;
  monikor_unregister_io_handler(handler->server->mon, handler->handler);
  monikor_server_handler_free(handler->handler);
}
