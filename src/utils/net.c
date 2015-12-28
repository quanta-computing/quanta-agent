#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "monikor.h"


int monikor_net_connect(const char *host, const char *port) {
  int sock = -1;
  struct addrinfo hints;
  struct addrinfo *ai_list;
  struct addrinfo *ai;

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;
  if (getaddrinfo(host, port, &hints, &ai_list))
    return -1;
  for (ai = ai_list; ai; ai = ai->ai_next) {
    if ((sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) != -1
    && !connect(sock, ai->ai_addr, ai_>ai_addrlen))
      break;
    else
      sock = -1;
  }
  freeaddrinfo(ai_list);
  return sock;
}

int monikor_net_send_data(int sock, const void *data, size_t len) {
  return send(sock, data, len, 0);
}

int monikor_net_send(int sock, const char *msg) {
  return monikor_net_send_data(sock, msg, strlen(msg));
}

char *monikor_net_recv(int sock) {
  char *response;
  size_t response_size = 0;
  int read_bytes;

  do {
    if (!(response = realloc(response, response_size + MNK_NET_RD_SZ + 1)))
      goto err;
    read_bytes = recv(sock, response + response_size, MNK_NET_RD_SZ, MSG_WAITALL);
    if (read_bytes == -1)
      goto err;
    response_size += read_bytes;
    response[response_size] = 0;
  } while (read_bytes == MNK_NET_RD_SZ);
  return response;

err:
  free(response);
  return NULL;
}

// Send & receive
char *monikor_net_sr(int sock, const char *msg) {
  if (monikor_net_send(sock, msg) == -1)
    return NULL;
  return monikor_net_recv(sock);
}

// Connect, send, receive
char *monikor_net_csr(const char *host, const char *port, const char *msg) {
  int sock;
  char *data;

  if ((sock = monikor_net_connect(host, port)) == -1)
    return NULL;
  data = monikor_net_sr(sock, msg);
  close(sock);
  return data;
}
