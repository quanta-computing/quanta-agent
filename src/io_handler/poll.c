#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/select.h>

#include "monikor.h"
#include "io_handler.h"

static int set_fd_sets(monikor_io_handler_list_t *list, fd_set *rdfds, fd_set *wrfds) {
  int ndfs = -1;

  FD_ZERO(rdfds);
  FD_ZERO(wrfds);
  curl_multi_fdset(list->curl.curl, rdfds, wrfds, NULL, &ndfs);
  for (monikor_io_handler_t *handler = list->first; handler; handler = handler->next) {
    if (handler->fd == -1)
      continue;
    if (handler->mode & MONIKOR_IO_HANDLER_RD)
      FD_SET(handler->fd, rdfds);
    else if (handler->mode & MONIKOR_IO_HANDLER_WR)
      FD_SET(handler->fd, wrfds);
    else
      continue;
    if (handler->fd > ndfs)
      ndfs = handler->fd;
  }
  return ndfs + 1;
}

static void update_timeout_if_needed(monikor_io_handler_list_t *list, struct timeval *timeout) {
  long curl_timeout = 0;

  curl_multi_timeout(list->curl.curl, &curl_timeout);
  if (curl_timeout > 0 && curl_timeout / 1000 < timeout->tv_sec) {
    timeout->tv_sec = curl_timeout / 1000;
    timeout->tv_usec = 1000 * (curl_timeout % 1000);
  }
}

static void perform_curl_handling(monikor_io_handler_list_t *list) {
  int running = 0;
  int msg_count = 0;
  CURLMsg *msg;
  CURL *easy_handler;
  monikor_curl_handler_t *handler;

  curl_multi_perform(list->curl.curl, &running);
  while ((msg = curl_multi_info_read(list->curl.curl, &msg_count))) {
    if (msg->msg != CURLMSG_DONE)
      continue;
    easy_handler = msg->easy_handle;
    for (handler = list->curl.first
    ; handler && handler->curl != easy_handler
    ; handler = handler->next);
    if (handler) {
      monikor_io_handler_list_remove_curl(list, handler);
      if (handler->callback)
        handler->callback(handler, msg->data.result);
      free(handler);
    } else {
      monikor_log(LOG_WARNING, "CURL handler not found in list, removing from multi handler...\n");
      curl_multi_remove_handle(list->curl.curl, easy_handler);
    }
    curl_easy_cleanup(easy_handler);
  }
}

int monikor_io_handler_poll(monikor_io_handler_list_t *list, struct timeval *timeout) {
  int ndfs;
  int err;
  fd_set rdfds;
  fd_set wrfds;
  monikor_io_handler_t *handler;
  monikor_io_handler_t *next;

  perform_curl_handling(list);
  ndfs = set_fd_sets(list, &rdfds, &wrfds);
  update_timeout_if_needed(list, timeout);
  monikor_log(LOG_DEBUG, "Sleeping for %d seconds\n", timeout->tv_sec);
  if (ndfs <= 0)
    return ndfs;
  do {
    err = select(ndfs, &rdfds, &wrfds, NULL, timeout);
  } while (err == -1 && errno == EINTR);
  if (err <= 0)
    return err;
  handler = list->first;
  while (handler) {
    next = handler->next;
    if (handler->fd != -1) {
      uint8_t mode = (FD_ISSET(handler->fd, &rdfds) ? MONIKOR_IO_HANDLER_RD : 0)
        | (FD_ISSET(handler->fd, &wrfds) ? MONIKOR_IO_HANDLER_WR : 0);
      if (mode && handler->callback) {
        handler->callback(handler, mode);
      }
    }
    handler = next;
  }
  return err;
}
