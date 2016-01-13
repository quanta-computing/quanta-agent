#ifndef MONIKOR_UTILS_H_H
#define MONIKOR_UTILS_H_H

#include <stdlib.h>
#include <stdint.h>

char      *monikor_read_file(const char *filepath);
int       monikor_net_connect(const char *host, const char *port);
int       monikor_net_send_data(int sock, const void *data, size_t len);
int       monikor_net_send(int sock, const char *msg);
char      *monikor_net_recv(int sock);
char      *monikor_net_sr(int sock, const char *msg);
char      *monikor_net_csr(const char *host, const char *port, const char *msg);

int timecmp(const struct timeval *a, const struct timeval *b);

// DEBUG stuff
void dump_metric_list(monikor_metric_list_t *list);


#endif /* end of include guard: MONIKOR_UTILS_H_H */
