#include <stdlib.h>

#include "config.h"

monikor_config_dict_t *monikor_config_dict_new(void) {
  monikor_config_dict_t *dict;

  if (!(dict = malloc(sizeof(*dict))))
    return NULL;
  dict->key = NULL;
  dict->type = MONIKOR_CFG_DICT;
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
    free(elm->key);
    switch (elm->type) {
      case MONIKOR_CFG_DICT:
        monikor_config_dict_free(elm->value.dict);
        break;
      case MONIKOR_CFG_SCALAR:
        free(elm->value.value);
        break;
      case MONIKOR_CFG_LIST:
        strl_delete(elm->value.list);
        break;
      default:
        break;
    }
    next = elm->next;
    free(elm);
  }
}

static monikor_config_dict_t *monikor_config_get_subdict(monikor_config_dict_t *dict, strl_t *keys) {
  strl_node_t *k;
  monikor_config_dict_t *sub = dict;

  if (!keys || !dict)
    return NULL;
  for (k = keys->first; k; k = k->next) {
    while (sub && (!sub->key || strcmp(k->str, sub->key)))
      sub = sub->next;
    if (sub && k == keys->last)
      return sub;
    if (!sub || sub->type != MONIKOR_CFG_DICT)
      return NULL;
    sub = sub->value.dict;
  }
  return sub;
}

monikor_config_dict_t *monikor_config_dict_get_dict(monikor_config_dict_t *dict, const char *key) {
  strl_t *keys;
  monikor_config_dict_t *d;

  if (!(keys = strl_from_separated_string(key, "."))) {
    return NULL;
  }
  d = monikor_config_get_subdict(dict, keys);
  strl_delete(keys);
  if (!d || d->type != MONIKOR_CFG_DICT)
    return NULL;
  return d;
}

strl_t *monikor_config_dict_get_list(monikor_config_dict_t *dict, const char *key) {
  strl_t *keys;
  monikor_config_dict_t *d;

  if (!(keys = strl_from_separated_string(key, "."))) {
    return NULL;
  }
  d = monikor_config_get_subdict(dict, keys);
  strl_delete(keys);
  if (!d || d->type != MONIKOR_CFG_LIST)
    return NULL;
  return d->value.list;
}

char *monikor_config_dict_get_scalar(monikor_config_dict_t *dict, const char *key) {
  strl_t *keys;
  monikor_config_dict_t *d;

  if (!(keys = strl_from_separated_string(key, "."))) {
    return NULL;
  }
  d = monikor_config_get_subdict(dict, keys);
  strl_delete(keys);
  if (!d || d->type != MONIKOR_CFG_SCALAR)
    return NULL;
  return d->value.value;
}
