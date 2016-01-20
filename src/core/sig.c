#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include "monikor.h"


/*
** This refers to the write end of the pipe used to communicate between signal handlers and main
** process select(2)
** It is initialized by monikor_register_signals and close(2) by monikor_signal_cleanup
*/
static int sig_writefd = -1;


static void handle_sigaction(int signo, siginfo_t *info, void *ctx) {
  (void)signo;
  (void)ctx;
  if (write(sig_writefd, info, sizeof(*info)) != sizeof(*info)) {}
}

static void handle_signal(monikor_io_handler_t *handler, uint8_t mode) {
  monikor_t *mon = (monikor_t *)handler->data;
  siginfo_t info;

  (void)mode;
  if (read(handler->fd, &info, sizeof(info)) != sizeof(info))
    return;
  monikor_log(LOG_INFO, "Got signal: %s\n", strsignal(info.si_signo));
  switch (info.si_signo) {
  case SIGINT:
  case SIGTERM:
  case SIGQUIT:
    monikor_exit(mon);
    break;
  case SIGHUP:
    monikor_flag_reload(mon);
    break;
  case SIGUSR1:
    monikor_send_all(mon);
    break;
  case SIGUSR2:
    monikor_flush_all(mon);
    break;
  }
}

int monikor_register_signals(monikor_t *mon) {
  int pipefds[2];
  struct sigaction sa;
  monikor_io_handler_t *handler;

  if (pipe(pipefds) == -1)
    return -1;
  handler = monikor_io_handler_new(pipefds[0], MONIKOR_IO_HANDLER_RD, &handle_signal, (void *)mon);
   if (!handler)
    return -1;
  sig_writefd = pipefds[1];
  sa.sa_flags = SA_RESTART | SA_SIGINFO;
  sa.sa_sigaction = &handle_sigaction;
  sigfillset(&sa.sa_mask);
  if (sigaction(SIGTERM, &sa, NULL) == -1
  ||  sigaction(SIGINT , &sa, NULL) == -1
  ||  sigaction(SIGQUIT, &sa, NULL) == -1
  ||  sigaction(SIGHUP , &sa, NULL) == -1
  ||  sigaction(SIGUSR1, &sa, NULL) == -1
  ||  sigaction(SIGUSR2, &sa, NULL) == -1)
    return -1;
  monikor_register_io_handler(mon, handler);
  return 0;
}

void monikor_signal_cleanup(void) {
  close(sig_writefd);
}
