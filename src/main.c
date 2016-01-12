#include <stdio.h>
#include <stdlib.h>

#include "monikor.h"

// int main(int argc, char **argv) {
//   monikor_t mon;
//
//   if (argc != 2)
//     usage();
//   if (monikor_init(&mon, argv[1]))
//     return 1;
//   monikor_run(&mon);
//   monikor_logger_cleanup();
//   return 0;
// }

#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
int main(int argc, char **argv) {
  int fd;

  if (argc != 3) {
    printf("Usage ./monikor [dump|read] <file>\n");
    exit(1);
  }
  if (!strcmp(argv[1], "dump")) {
    monikor_metric_list_t *list = monikor_metric_list_new();
    struct timeval clock;
    void *data;
    size_t size;

    gettimeofday(&clock, NULL);
    monikor_metric_list_push(list, monikor_metric_integer("toto", &clock, 42, 0));
    monikor_metric_list_push(list, monikor_metric_float("tata", &clock, 42.0, 0));
    monikor_metric_list_push(list, monikor_metric_string("tutu", &clock, "toto42sh"));
    monikor_metric_list_serialize(list, &data, &size);
    if ((fd = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0644)) == -1) {
      printf("File error\n");
      exit(1);
    }
    write(fd, data, size);
  } else {
    monikor_serialized_metric_list_hdr_t hdr;
    monikor_metric_list_t *metrics;
    uint8_t hdr_data[SERIALIZED_METRIC_LIST_HDR_SIZE];
    void *data;

    if ((fd = open(argv[2], O_RDONLY)) == -1) {
      printf("File error\n");
      exit(1);
    }
    read(fd, hdr_data, SERIALIZED_METRIC_LIST_HDR_SIZE);
    monikor_metric_list_header_unserialize((void *)hdr_data, &hdr);
    printf("GOT PROTOCOL VERSION %hhu. %hu metrics, size %hu\n", hdr.version, hdr.count, hdr.data_size);
    data = malloc(hdr.data_size);
    read(fd, data, hdr.data_size);
    monikor_metric_list_unserialize(data, &hdr, &metrics);
    free(data);
    dump_metric_list(metrics);
  }
}
