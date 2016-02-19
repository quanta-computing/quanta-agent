#include <stdlib.h>
#include <curl/curl.h>

#include "monikor.h"

#define MAX_HEADER_LENGTH 256
#define RESPONSE_BODY_SIZE 1024

typedef struct {
  struct curl_slist *headers;
  char response[RESPONSE_BODY_SIZE];
  char error[CURL_ERROR_SIZE];
  void *data;
  size_t data_size;
} send_data_t;

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

static void setup_curl_request(monikor_curl_handler_t *handler, const char *url, int timeout) {
  send_data_t *data = (send_data_t *)handler->data;

  strcpy(data->response, "Unknown error");
  data->error[0] = 0;
  curl_easy_setopt(handler->curl, CURLOPT_URL, url);
  curl_easy_setopt(handler->curl, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(handler->curl, CURLOPT_POSTFIELDSIZE, data->data_size);
  curl_easy_setopt(handler->curl, CURLOPT_POSTFIELDS, (char *)data->data);
  curl_easy_setopt(handler->curl, CURLOPT_HTTPHEADER, data->headers);
  curl_easy_setopt(handler->curl, CURLOPT_TIMEOUT, timeout);
  curl_easy_setopt(handler->curl, CURLOPT_WRITEDATA, (void *)data->response);
  curl_easy_setopt(handler->curl, CURLOPT_WRITEFUNCTION, &data_handler);
  curl_easy_setopt(handler->curl, CURLOPT_ERRORBUFFER, data->error);
}

static void handle_server_response(monikor_curl_handler_t *handler, CURLcode result) {
  send_data_t *data = (send_data_t *)handler->data;
  long http_code;

  curl_easy_getinfo(handler->curl, CURLINFO_RESPONSE_CODE, &http_code);
  if (result != CURLE_OK) {
    size_t errlen = strlen(data->error);
    if (errlen) {
      if (data->error[errlen - 1] == '\n')
        data->error[errlen - 1] = 0;
      monikor_log(LOG_ERR, "Send metrics failed: %s\n", data->error);
    } else {
      monikor_log(LOG_ERR, "Send metrics failed: %s\n", curl_easy_strerror(result));
    }
  } else if (http_code != 200) {
    monikor_log(LOG_ERR, "Send metrics failed: %s\n", data->response);
  } else {
    monikor_log(LOG_INFO, "Metrics successfully sent\n");
  }
  curl_slist_free_all(data->headers);
  free(data->data);
  free(data);
}

int monikor_send_metrics(monikor_t *mon) {
  monikor_curl_handler_t *handler = NULL;
  send_data_t *data = NULL;
  struct curl_slist *headers = NULL;

  if (!mon->metrics->current->size) {
    monikor_log(LOG_INFO, "No metrics to send\n");
    return 0;
  }
  if (!mon->config->server_url
  || !(data = malloc(sizeof(*data)))
  || monikor_metric_list_serialize(mon->metrics->current, &data->data, &data->data_size)
  || !(headers = set_http_headers(mon))
  || !(handler = monikor_curl_handler_new(&handle_server_response, data))) {
    monikor_log(LOG_ERR, "Cannot perform metric send\n");
    if (data)
      free(data->data);
    free(data);
    if (handler)
      curl_easy_cleanup(handler->curl);
    free(handler);
    curl_slist_free_all(headers);
    return -1;
  }
  data->headers = headers;
  setup_curl_request(handler, mon->config->server_url, mon->config->server_timeout);
  monikor_io_handler_list_push_curl(&mon->io_handlers, handler);
  monikor_log(LOG_INFO, "Sending %zu metrics to %s\n",
    mon->metrics->current->size, mon->config->server_url);
  return 0;
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
