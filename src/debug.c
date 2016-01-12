#include <stdio.h>

#include "monikor.h"

void dump_metric(monikor_metric_t *metric) {
  switch (metric->type) {
  case MONIKOR_INTEGER:
    printf("METRIC INT %s@%ld: %llu\n", metric->name, metric->clock.tv_sec, metric->value._int);
    break;
  case MONIKOR_FLOAT:
    printf("METRIC FLOAT %s@%ld: %f\n", metric->name, metric->clock.tv_sec, metric->value._float);
    break;
  case MONIKOR_STRING:
    printf("METRIC STRING %s@%ld: %s\n", metric->name, metric->clock.tv_sec, metric->value._string);
    break;
  default:
    printf("UNKNOWN METRIC %s@%ld\n", metric->name, metric->clock.tv_sec);
    break;
  }
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
