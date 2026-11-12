#include "monikor.h"
#include "io_handler.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>

#define MNK_NET_RD_SZ 4096
#define MNK_NET_MAX_RESPONSE_SZ (1024 * 1024)

typedef struct {
  char *send;
  char *response;
  size_t response_size;
  void (*callback)(char *, void *);
  void *data;
  struct addrinfo *ai;
  struct addrinfo *ai_cur;
} monikor_net_handler_data_t;


static int monikor_net_gai(const char *host, const char *port, monikor_net_handler_data_t *handler_data) {
  struct addrinfo hints;
  int err;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;
  handler_data->ai = NULL;
  handler_data->ai_cur = NULL;
  if ((err = getaddrinfo(host, port, &hints, &handler_data->ai))) {
    monikor_log(LOG_ERR, "Cannot resolve %s port %s: %s\n", host, port, gai_strerror(err));
    return -1;
  }
  handler_data->ai_cur = handler_data->ai;
  return 0;
}

static int monikor_net_connect(monikor_net_handler_data_t *handler_data) {
  int sock;
  int flags;
  int saved_errno;
  struct addrinfo *ai = handler_data->ai_cur;
  char host[512];
  char port[512];

  if (!getnameinfo(ai->ai_addr, ai->ai_addrlen, host, 512, port, 512, NI_NUMERICHOST|NI_NUMERICSERV))
    monikor_log(LOG_DEBUG, "Trying to connect to %s port %s...\n", host, port);
  if ((sock = socket(ai->ai_family, ai->ai_socktype | SOCK_CLOEXEC, ai->ai_protocol)) == -1
  || (flags = fcntl(sock, F_GETFL, 0)) == -1 || fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1
  || (connect(sock, ai->ai_addr, ai->ai_addrlen) == -1 && errno != EINPROGRESS)) {
    if (sock != -1) {
      saved_errno = errno;
      close(sock);
      errno = saved_errno;
    }
    return -1;
  }
  return sock;
}

static int monikor_net_connect_any(monikor_net_handler_data_t *handler_data) {
  int fd;

  for (; handler_data->ai_cur; handler_data->ai_cur = handler_data->ai_cur->ai_next) {
    if ((fd = monikor_net_connect(handler_data)) != -1)
      return fd;
  }
  return -1;
}

// Returns 0 on EOF (response complete), 1 when the socket would block, -1 on error
static int monikor_net_recv(int sock, monikor_net_handler_data_t *handler_data) {
  char *response;
  ssize_t read_bytes;

  do {
    if (handler_data->response_size > MNK_NET_MAX_RESPONSE_SZ) {
      errno = EFBIG;
      return -1;
    }
    if (!(response = realloc(handler_data->response, handler_data->response_size + MNK_NET_RD_SZ + 1)))
      return -1;
    handler_data->response = response;
    response[handler_data->response_size] = 0;
    read_bytes = read(sock, response + handler_data->response_size, MNK_NET_RD_SZ);
    if (read_bytes > 0) {
      handler_data->response_size += read_bytes;
      response[handler_data->response_size] = 0;
    }
  } while (read_bytes > 0 || (read_bytes == -1 && errno == EINTR));
  if (read_bytes == 0)
    return 0;
  return errno == EAGAIN || errno == EWOULDBLOCK ? 1 : -1;
}

static void handle_io_read(monikor_io_handler_t *handler, uint8_t mode) {
  monikor_net_handler_data_t *handler_data = (monikor_net_handler_data_t *)handler->data;
  char *received;
  void *data;
  void (*callback)(char *, void *);
  int status;

  (void)mode;
  if ((status = monikor_net_recv(handler->fd, handler_data)) == 1)
    return;
  if (status == -1)
    monikor_log(LOG_ERR, "Cannot receive data: %s\n", strerror(errno));
  received = handler_data->response;
  data = handler_data->data;
  callback = handler_data->callback;
  close(handler->fd);
  handler->fd = -1;
  handler->mode = 0;
  handler->callback = NULL;
  freeaddrinfo(handler_data->ai);
  free(handler->data);
  handler->data = NULL;
  if (!status && received)
    callback(received, data);
  free(received);
}

static void handle_io_connect(monikor_io_handler_t *handler, uint8_t mode) {
  monikor_net_handler_data_t *handler_data = (monikor_net_handler_data_t *)handler->data;
  size_t send_len;
  ssize_t written;
  int result;
  int fd;
  socklen_t result_len = sizeof(result);

  (void)mode;
  if (getsockopt(handler->fd, SOL_SOCKET, SO_ERROR, &result, &result_len) == -1 || result) {
    handler_data->ai_cur = handler_data->ai_cur->ai_next;
    if ((fd = monikor_net_connect_any(handler_data)) == -1) {
      monikor_log(LOG_ERR, "Cannot connect: No more addresses to try\n");
      goto err;
    }
    close(handler->fd);
    handler->fd = fd;
    return;
  }
  if (handler_data->send && (send_len = strlen(handler_data->send))
  && (written = write(handler->fd, handler_data->send, send_len)) != (ssize_t)send_len) {
    monikor_log(LOG_ERR, "Cannot send data: %s\n",
      written == -1 ? strerror(errno) : "short write");
    goto err;
  }
  handler->mode = MONIKOR_IO_HANDLER_RD;
  handler->callback = &handle_io_read;
  return;

err:
    handler->mode = 0;
    close(handler->fd);
    handler->fd = -1;
    freeaddrinfo(handler_data->ai);
    free(handler_data);
    handler->data = NULL;
    handler->callback = NULL;
}

monikor_io_handler_t *monikor_net_exchange(const char *host, const char *port, char *send,
void (*callback)(char *response, void *data), void *data) {
  monikor_net_handler_data_t *handler_data;
  monikor_io_handler_t *handler;
  int fd;

  if (!(handler_data = malloc(sizeof(*handler_data)))
  || monikor_net_gai(host, port, handler_data)) {
    free(handler_data);
    return NULL;
  }
  if ((fd = monikor_net_connect_any(handler_data)) == -1) {
    freeaddrinfo(handler_data->ai);
    free(handler_data);
    return NULL;
  }
  handler = monikor_io_handler_new(fd, MONIKOR_IO_HANDLER_WR, &handle_io_connect,
    (void *)handler_data);
  if (!handler) {
    freeaddrinfo(handler_data->ai);
    free(handler_data);
    close(fd);
    return NULL;
  }
  handler_data->send = send;
  handler_data->response = NULL;
  handler_data->response_size = 0;
  handler_data->callback = callback;
  handler_data->data = data;
  return handler;
}

void monikor_net_exchange_free(monikor_io_handler_t *handler) {
  monikor_net_handler_data_t *handler_data = (monikor_net_handler_data_t *)handler->data;

  if (handler->fd != -1)
    close(handler->fd);
  if (handler_data) {
    free(handler_data->response);
    freeaddrinfo(handler_data->ai);
    free(handler_data);
  }
  free(handler);
}
