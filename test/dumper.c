/*
** Debug agent which prints out the metrics it receives on the unix socket
** Compile with: gcc -W -Wall -Wextra -lmonikor dumper.c -o dumper
** Run with: ./dumper /path/to/unix.sock
** Do not forget to update quanta_mon.ini accordingly.
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

#include <monikor/metric.h>

static void dump_metric(monikor_metric_t *metric, __attribute__((unused)) void *data) {
  switch (metric->type) {
  case MONIKOR_INTEGER:
    printf("METRIC INT %s@%ld: %"PRIu64"\n",
      metric->name, metric->clock.tv_sec, metric->value._int);
    break;
  case MONIKOR_FLOAT:
    printf("METRIC FLOAT %s@%ld: %f\n",
      metric->name, metric->clock.tv_sec, metric->value._float);
    break;
  case MONIKOR_STRING:
    printf("METRIC STRING %s@%ld: %s\n",
      metric->name, metric->clock.tv_sec, metric->value._string);
    break;
  default:
    printf("UNKNOWN METRIC %s@%ld\n",
      metric->name, metric->clock.tv_sec);
    break;
  }
}


static int start_unix_srv(const char *unix_sock_path) {
  struct sockaddr_un addr;
  int fd;

  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, unix_sock_path);
  unlink(unix_sock_path);
  if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1
  || bind(fd, (struct sockaddr *)&addr, sizeof(addr))
  || chmod(unix_sock_path, 0666)
  || listen(fd, 10))
    return -1;
  return fd;
}


static void handle_client(int fd) {
  char buf[SERIALIZED_METRIC_LIST_HDR_SIZE];
  char *data;
  size_t rd;
  monikor_serialized_metric_list_hdr_t header;
  monikor_metric_list_t *metrics;

  printf("RECEIVING DATA FROM CLIENT on fd %d\n", fd);
  read(fd, buf, SERIALIZED_METRIC_LIST_HDR_SIZE);
  monikor_metric_list_header_unserialize((void *)buf, &header);
  data = malloc(header.data_size);
  rd = 0;
  do {
    rd += read(fd, data + rd, header.data_size - rd);
  } while (rd < header.data_size);
  monikor_metric_list_unserialize(data, &header, &metrics);
  monikor_metric_list_apply(metrics, &dump_metric, NULL);
  monikor_metric_list_free(metrics);
  printf("-------------------------------------\n\n");
}


int main(int argc, char *argv[]) {
  int server_fd;
  int client_fd;
  struct sockaddr_un addr;
  socklen_t addrlen = sizeof(addr);

  if (argc != 2) {
    fprintf(stderr, "Usage: dumper <socket_path>\n");
    exit(1);
  }
  server_fd = start_unix_srv(argv[1]);
  while (42) {
    client_fd = accept(server_fd, (struct sockaddr *)&addr, &addrlen);
    handle_client(client_fd);
    close(client_fd);
  }
}
