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

  if (!(new_data = realloc(response->data, response->size + data_size))) {
    free(response->data);
    response->data = NULL;
    return 0;
  }
  response->data = new_data;
  memcpy((void *)(response->data + response->size), (void *)ptr, data_size);
  response->size += data_size;
  return data_size;
}

http_response_t *monikor_http_get(const char *url, long timeout) {
  CURL *curl;
  http_response_t *response;

  if (!(response = new_response())
  || !(curl = curl_easy_init())) {
    free(response);
    return NULL;
  }
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_handler);
  if (curl_easy_perform(curl) != CURLE_OK) {
    free(response->data);
    free(response);
    curl_easy_cleanup(curl);
    return NULL;
  }
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->code);
  curl_easy_cleanup(curl);
  return response;
}
