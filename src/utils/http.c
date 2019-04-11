#include <string.h>
#include <curl/curl.h>

#include "monikor.h"
#include "utils.h"


static http_response_t *new_response(void) {
  http_response_t *r;

  if (!(r = malloc(sizeof(*r))))
    return NULL;
  r->code = 0;
  r->size = 0;
  r->data = NULL;
  return r;
}

static size_t data_handler(char *ptr, size_t size, size_t nmemb, void *data) {
  http_response_t *response = (http_response_t *)data;
  char *new_data;
  size_t data_size = size * nmemb;

  if (!(new_data = realloc(response->data, response->size + data_size + 1))) {
    free(response->data);
    response->data = NULL;
    return 0;
  }
  response->data = new_data;
  memcpy((void *)(response->data + response->size), (void *)ptr, data_size);
  response->size += data_size;
  response->data[response->size] = 0;
  return data_size;
}

static void handle_server_response(monikor_curl_handler_t *handler, CURLcode result) {
  http_response_t *response = (http_response_t *)handler->data;

  curl_easy_getinfo(handler->curl, CURLINFO_RESPONSE_CODE, &response->code);
  response->callback(response, result);
}

int monikor_http_get(monikor_t *mon, const char *url, long timeout,
void (*callback)(http_response_t *response, CURLcode result), void *userdata) {
  return monikor_http_get_with_headers(mon, url, timeout, callback, userdata, NULL);
}

int monikor_http_get_with_headers(monikor_t *mon, const char *url, long timeout,
void (*callback)(http_response_t *response, CURLcode result), void *userdata, struct curl_slist *headers) {
  monikor_curl_handler_t *handler;
  http_response_t *response;

  if (!(response = new_response())
  || !(handler = monikor_curl_handler_new(&handle_server_response, (void *)response))
  ) {
    free(response);
    return 1;
  }
  response->callback = callback;
  response->userdata = userdata;
  curl_easy_setopt(handler->curl, CURLOPT_URL, url);
  curl_easy_setopt(handler->curl, CURLOPT_TIMEOUT, timeout);
  curl_easy_setopt(handler->curl, CURLOPT_WRITEDATA, (void *)response);
  curl_easy_setopt(handler->curl, CURLOPT_WRITEFUNCTION, &data_handler);
  if (headers)
    curl_easy_setopt(handler->curl, CURLOPT_HTTPHEADER, headers);
  monikor_io_handler_list_push_curl(&mon->io_handlers, handler);
  return 0;
}
