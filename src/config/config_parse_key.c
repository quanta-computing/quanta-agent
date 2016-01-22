#include <stdio.h>
#include <yaml.h>

#include "config.h"
#include "strl.h"


int config_parse_key(yaml_parser_t *parser, const char *key, monikor_config_dict_t *dict) {
  yaml_event_t ev;
  monikor_config_dict_t *sub;

  dict->key = strdup(key);
  if (!yaml_parser_parse(parser, &ev))
    return 1;
  switch (ev.type) {
    case YAML_SCALAR_EVENT:
      dict->type = MONIKOR_CFG_SCALAR;
      dict->value.value = strdup((char *)ev.data.scalar.value);
      break;

    case YAML_SEQUENCE_START_EVENT:
      dict->type = MONIKOR_CFG_LIST;
      if (!(dict->value.list = strl_new())
      || config_parse_list(parser, dict->value.list)) {
        printf("bad return from parse_list\n");
        yaml_event_delete(&ev);
        return 1;
      }
      break;


    case YAML_MAPPING_START_EVENT:
      if (!(sub = monikor_config_dict_new())
      || config_parse_mapping(parser, sub)) {
        printf("bad return from parse_mapping\n");
        yaml_event_delete(&ev);
        return 1;
      }
      dict->type = MONIKOR_CFG_DICT;
      dict->value.dict = sub;
      break;

    default:
      printf("error in parse_key, event %d\n", ev.type);
      yaml_event_delete(&ev);
      return 1;
  }
  yaml_event_delete(&ev);
  return 0;
}
