#include <stdio.h>

#include "monikor.h"

void dump_metric(monikor_metric_t *metric) {
  switch (metric->type) {
    case MONIKOR_INTEGER:
      printf("METRIC %s@%ld: %ld\n", metric->name, metric->clock, metric->value._int);
      break;
    case MONIKOR_FLOAT:
      printf("METRIC %s@%ld: %f\n", metric->name, metric->clock, metric->value._float);
      break;
    case MONIKOR_STRING:
      printf("METRIC %s@%ld: %s\n", metric->name, metric->clock, metric->value._string);
      break;
  }
}

void dump_metric_list(monikor_metric_list_t *list) {
  monikor_metric_list_apply(list, dump_metric);
}

void dump_config(monikor_config_dict_t *dict) {
  for (; dict; dict = dict->next) {
    switch (dict->type) {
      case DICT:
        printf("dict %s\n", dict->key);
        dump_config(dict->value.dict);
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
