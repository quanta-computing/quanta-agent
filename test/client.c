#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <fcntl.h>

#include "metric.h"

int main(int argc, char **argv) {
  int sock;
  struct sockaddr_un addr;

  if (argc != 2) {
    printf("Usage ./client <sock_path>\n");
    exit(1);
  }
  monikor_metric_list_t *list = monikor_metric_list_new();
  struct timeval clock;
  void *data;
  size_t size;

  gettimeofday(&clock, NULL);
  monikor_metric_list_push(list, monikor_metric_integer("client_int", &clock, 42, 0));
  monikor_metric_list_push(list, monikor_metric_float("client_float", &clock, 42.0, 0));
  monikor_metric_list_push(list, monikor_metric_string("client_string", &clock, "toto42sh"));
  monikor_metric_list_serialize(list, &data, &size);
  sock = socket(AF_UNIX, SOCK_STREAM, 0);
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, argv[1]);
  connect(sock, (struct sockaddr *)&addr, sizeof(addr));
  int w = write(sock, data, size);
  close(sock);
  free(data);
  return w == -1;
}
