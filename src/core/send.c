#include <stdlib.h>
#include <curl/curl.h>

#include "monikor.h"

#define MAX_HEADER_LENGTH 256
#define RESPONSE_BODY_SIZE 1024

size_t data_handler(char *ptr, size_t size, size_t nmemb, void *userdata) {
  char *data = userdata;

  strncpy(data, ptr, RESPONSE_BODY_SIZE > size * nmemb ? size * nmemb : RESPONSE_BODY_SIZE);
  data[RESPONSE_BODY_SIZE] = 0;
  return size * nmemb;
}

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

static CURL *setup_curl_request(monikor_t *mon, struct curl_slist *headers, char *error,
char *response, void *data, size_t data_size) {
  CURL *curl;

  strcpy(response, "Unknown error");
  *error = 0;
  if (!(curl = curl_easy_init()))
    return NULL;
  curl_easy_setopt(curl, CURLOPT_URL, mon->config->server_url);
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_size);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (char *)data);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, mon->config->server_timeout);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_handler);
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error);
  return curl;
}

static int perform_curl_request(CURL *curl, char *error, char *response) {
  long http_code;
  CURLcode result;

  result = curl_easy_perform(curl);
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  if (result != CURLE_OK) {
    size_t errlen = strlen(error);
    if (errlen) {
      if (error[errlen - 1] == '\n')
        error[errlen - 1] = 0;
      monikor_log(LOG_ERR, "Send metrics failed: %s\n", error);
    } else {
      monikor_log(LOG_ERR, "Send metrics failed: %s\n", curl_easy_strerror(result));
    }
    return 1;
  } else if (http_code != 200) {
    monikor_log(LOG_ERR, "Send metrics failed: %s\n", response);
    return 1;
  } else {
    monikor_log(LOG_INFO, "Metrics successfully sent\n");
    return 0;
  }
}

int monikor_send_metrics(monikor_t *mon) {
  CURL *curl;
  int ret;
  void *data = NULL;
  size_t data_size;
  char response[RESPONSE_BODY_SIZE + 1];
  char error[CURL_ERROR_SIZE];
  struct curl_slist *headers = NULL;

  if (!mon->metrics->current->size) {
    monikor_log(LOG_INFO, "No metrics to send\n");
    return 0;
  }
  if (!mon->config->server_url
  || monikor_metric_list_serialize(mon->metrics->current, &data, &data_size)
  || !(headers = set_http_headers(mon))
  || !(curl = setup_curl_request(mon, headers, error, response, data, data_size))) {
    monikor_log(LOG_ERR, "Cannot perform metric send\n");
    free(data);
    curl_slist_free_all(headers);
    return -1;
  }
  monikor_log(LOG_INFO, "Sending %zu metrics to %s\n",
    mon->metrics->current->size, mon->config->server_url);
  ret = perform_curl_request(curl, error, response);
  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);
  free(data);
  return ret;
}

void monikor_send_all_metrics(monikor_t *mon) {
  monikor_metric_list_concat(mon->metrics->current, mon->metrics->cache);
  if (!monikor_send_metrics(mon)) {
    monikor_metric_store_flush(mon->metrics);
  } else {
    monikor_log(LOG_DEBUG, "Caching %zu metrics\n", mon->metrics->current->size);
    monikor_metric_store_cache(mon->metrics);
  }
}
