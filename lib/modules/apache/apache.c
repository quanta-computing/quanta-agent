#include <stdlib.h>
#include <stdio.h>

#include "monikor.h"
#include "apache.h"


void *apache_init(monikor_t *mon, monikor_config_dict_t *config) {
  char *url;
  char *config_url;

  (void)mon;
  config_url = monikor_config_dict_get_scalar(config, "apache.url");
  if (!config_url) {
    url = strdup(APACHE_DEFAULT_STATUS_URL);
  } else {
    size_t url_len = strlen(config_url);
    size_t auto_len = strlen("?auto");
    if (url_len <= auto_len || strncmp(config_url + url_len - auto_len, "?auto", auto_len)) {
      if (asprintf(&url, "%s?auto", config_url) == -1)
        return NULL;
    } else {
      url = strdup(config_url);
    }
  }
  return (void *)url;
}

void apache_cleanup(monikor_t *mon, void *data) {
  (void)mon;
  free(data);
}

int apache_poll(monikor_t *mon, void *data) {
  struct timeval now;

  if (!data)
    return -1;
  gettimeofday(&now, NULL);
  return apache_poll_metrics(mon, &now, (char *)data);
}
