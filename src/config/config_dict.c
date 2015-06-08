#include <stdlib.h>

#include "config.h"

monikor_config_dict_t *monikor_config_dict_new(void) {
  monikor_config_dict_t *dict;

  if (!(dict = malloc(sizeof(*dict))))
    return NULL;
  dict->key = NULL;
  dict->type = DICT;
  dict->value.dict = NULL;
  dict->next = NULL;
  return dict;
}

void monikor_config_dict_free(monikor_config_dict_t *dict) {
  monikor_config_dict_t *elm;
  monikor_config_dict_t *next = NULL;

  if (!dict)
    return;
  for (elm = dict; elm; elm = next) {
    free(dict->key);
    switch (dict->type) {
      case DICT:
        monikor_config_dict_free(dict->value.dict);
        break;
      case SCALAR:
        free(dict->value.value);
        break;
      default:
        break;
    }
    next = elm->next;
    free(elm);
  }
}
