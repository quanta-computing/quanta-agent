#include <stdlib.h>
#include <curl/curl.h>

#include "monikor.h"

#define MAX_HEADER_LENGTH 256

static struct curl_slist *set_http_headers(monikor_t *mon) {
  struct curl_slist *headers = NULL;
  char hdr[MAX_HEADER_LENGTH];


  if (!(headers = curl_slist_append(headers, "Content-Type: application/octet-stream")))
    return NULL;
  sprintf(hdr, "Authorization: Token %s", mon->config->quanta_token);
  if (!(headers = curl_slist_append(headers, hdr)))
    return NULL;
  sprintf(hdr, "X-Quanta-Hostid: %s", mon->config->hostid);
  if (!(headers = curl_slist_append(headers, hdr)))
    return NULL;
  return headers;
}

int monikor_send_metrics(monikor_t *mon) {
  CURL *curl;
  CURLcode result;
  long http_code;
  struct curl_slist *headers = NULL;
  void *data;
  size_t data_size;

  monikor_metric_list_concat(mon->metrics->current, mon->metrics->cache);
  if (!mon->config->server_url
  || monikor_metric_list_serialize(mon->metrics->current, &data, &data_size)
  || !(headers = set_http_headers(mon))
  || !(curl = curl_easy_init())) {
    curl_slist_free_all(headers);
    return -1;
  }
  monikor_log(LOG_INFO, "Sending %zu metrics to %s\n", mon->metrics->current->size, mon->config->server_url);
  curl_easy_setopt(curl, CURLOPT_URL, mon->config->server_url);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_size);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (char *)data);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, mon->config->server_timeout);
  result = curl_easy_perform(curl);
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);
  free(data);
  return !(result == CURLE_OK && http_code == 200);
}
