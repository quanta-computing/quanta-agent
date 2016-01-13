#ifndef MONIKOR_IO_HANDLER_H_
#define MONIKOR_IO_HANDLER_H_

#include <stdint.h>

#define MONIKOR_IO_HANDLER_RD (1 << 0)
#define MONIKOR_IO_HANDLER_WR (1 << 1)
#define MONIKOR_IO_HANDLER_RDWR (IO_HANDLER_RD | IO_HANDLER_WR)

typedef struct _monikor_io_handler {
  int fd;
  uint8_t mode;
  void (*callback)(struct _monikor_io_handler *handler, uint8_t mode);
  void *data;

  // Internal use only
  struct _monikor_io_handler *prev;
  struct _monikor_io_handler *next;
} monikor_io_handler_t;


typedef struct {
  monikor_io_handler_t *first;
  monikor_io_handler_t *last;
} monikor_io_handler_list_t;

monikor_io_handler_t *monikor_io_handler_new(int fd, uint8_t mode,
  void (*callback)(monikor_io_handler_t *handler, uint8_t mode), void *data);

void monikor_io_handler_list_init(monikor_io_handler_list_t *list);
void monikor_io_handler_list_push(monikor_io_handler_list_t *list, monikor_io_handler_t *handler);
void monikor_io_handler_list_remove(monikor_io_handler_list_t *list, monikor_io_handler_t *handler);

int monikor_io_handler_poll(monikor_io_handler_list_t *list, struct timeval *timeout);

#endif /* end of include guard: MONIKOR_IO_HANDLER_H_ */
