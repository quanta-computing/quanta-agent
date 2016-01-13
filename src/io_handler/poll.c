#include <stdlib.h>
#include <sys/socket.h>
#include <sys/select.h>

#include "io_handler.h"

static int set_fd_sets(monikor_io_handler_list_t *list, fd_set *rdfds, fd_set *wrfds) {
  int ndfs = -1;

  FD_ZERO(rdfds);
  FD_ZERO(wrfds);
  for (monikor_io_handler_t *handler = list->first; handler; handler = handler->next) {
    if (handler->mode & MONIKOR_IO_HANDLER_RD)
      FD_SET(handler->fd, rdfds);
    if (handler->mode & MONIKOR_IO_HANDLER_WR)
      FD_SET(handler->fd, wrfds);
    if (handler->fd > ndfs)
      ndfs = handler->fd;
  }
  return ndfs + 1;
}

int monikor_io_handler_poll(monikor_io_handler_list_t *list, struct timeval *timeout) {
  int ndfs;
  int err;
  fd_set rdfds;
  fd_set wrfds;

  ndfs = set_fd_sets(list, &rdfds, &wrfds);
  if (ndfs <= 0)
    return ndfs;
  err = select(ndfs, &rdfds, &wrfds, NULL, timeout);
  if (err <= 0)
    return err;
  for (monikor_io_handler_t *handler = list->first; handler; handler = handler->next) {
    uint8_t mode = (FD_ISSET(handler->fd, &rdfds) ? MONIKOR_IO_HANDLER_RD : 0)
      | (FD_ISSET(handler->fd, &wrfds) ? MONIKOR_IO_HANDLER_WR : 0);
    if (mode)
      handler->callback(handler, mode);
  }
  return err;
}
