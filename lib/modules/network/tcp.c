#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <inttypes.h>

#include "monikor.h"
#include <stdio.h>
#include <stdlib.h>
#include "network.h"

int find_index(char *str, char *name) {
  char *pos;
  int n = 0;

  if (!(pos = strstr(str, name)))
    return 0;
  for (int i = 0; str + i != pos; i++) {
    if (isspace(str[i]))
      n++;
  }
  return n;
}

int get_index_value(char *str, int index)
{
  int i = -1;
  int space = 0;

  if (index == 0)
    return 0;
  while (str[++i])
  {
    if (str[i] == ' ')
      space++;
    if (space == index)
      return strtoull(&str[i], NULL, 0);
  }
  return 0;
}

int poll_tcp_metrics(monikor_t *mon, struct timeval *clock)
{
  char *stats;
  char *stat;
  char *value;
  char *end;
  int n = 0;

  if (!(stats = monikor_read_file("/proc/net/snmp")))
    return 0;
  stat = strstr(stats, "Tcp:");
  value = strstr(stat + 1, "Tcp:");
  if (!(end = strchr(stat, '\n')))
    return 0;
  *end = '\0';
  if (!(end = strchr(value, '\n')))
    return 0;
  *end = 0;
  n += monikor_metric_push(mon, monikor_metric_integer("network.tcp.active",
    clock, get_index_value(value, find_index(stat, "ActiveOpens")), MONIKOR_METRIC_TIMEDELTA));
  n += monikor_metric_push(mon, monikor_metric_integer("network.tcp.passive",
    clock, get_index_value(value, find_index(stat, "PassiveOpens")), MONIKOR_METRIC_TIMEDELTA));
  n += monikor_metric_push(mon, monikor_metric_integer("network.tcp.failed",
    clock, get_index_value(value, find_index(stat, "AttemptFails")), MONIKOR_METRIC_TIMEDELTA));
  n += monikor_metric_push(mon, monikor_metric_integer("network.tcp.resets",
    clock, get_index_value(value, find_index(stat, "EstabResets")), MONIKOR_METRIC_TIMEDELTA));
  n += monikor_metric_push(mon, monikor_metric_integer("network.tcp.current",
    clock, get_index_value(value, find_index(stat, "CurrEstab")), 0));
  free(stats);
  return n;
}
