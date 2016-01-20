#include <stdio.h>
#include <inttypes.h>

#include "monikor.h"

void dump_metric(monikor_metric_t *metric) {
  switch (metric->type) {
  case MONIKOR_INTEGER:
    monikor_log(LOG_DEBUG, "METRIC INT %s@%ld: %"PRIu64"\n",
      metric->name, metric->clock.tv_sec, metric->value._int);
    break;
  case MONIKOR_FLOAT:
    monikor_log(LOG_DEBUG, "METRIC FLOAT %s@%ld: %f\n",
      metric->name, metric->clock.tv_sec, metric->value._float);
    break;
  case MONIKOR_STRING:
    monikor_log(LOG_DEBUG, "METRIC STRING %s@%ld: %s\n",
      metric->name, metric->clock.tv_sec, metric->value._string);
    break;
  default:
    monikor_log(LOG_DEBUG, "UNKNOWN METRIC %s@%ld\n",
      metric->name, metric->clock.tv_sec);
    break;
  }
}

void dump_store_size(monikor_metric_store_t *store) {
  monikor_log(LOG_DEBUG, "Current: %zu (%zu)\n\tDelta: %zu (%zu)\n\tCache: %zu (%zu)\n",
    store->current->size, store->current->data_size,
    store->delta->size, store->delta->data_size,
    store->cache->size, store->cache->data_size
  );
}

void dump_metric_list(monikor_metric_list_t *list) {
  monikor_metric_list_apply(list, dump_metric);
}

void dump_config_list_elm(char *s) {
  printf("%s, ", s);
}

void dump_config(monikor_config_dict_t *dict) {
  for (; dict; dict = dict->next) {
    switch (dict->type) {
      case DICT:
        printf("dict %s\n", dict->key);
        dump_config(dict->value.dict);
        break;
      case LIST:
        printf("list %s: ", dict->key);
        strl_apply(dict->value.list, dump_config_list_elm);
        printf("\n");
        break;
      case SCALAR:
        printf("%s = %s\n", dict->key, dict->value.value);
        break;
      default:
        printf("unknown\n");
        break;
    }
  }
}
