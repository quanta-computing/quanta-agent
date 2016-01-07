#include <sys/time.h>

int timecmp(const struct timeval *a, const struct timeval *b) {
  if (a->tv_sec != b->tv_usec)
    return a->tv_sec - b->tv_sec;
  else
   return a->tv_usec - b->tv_usec;
}
