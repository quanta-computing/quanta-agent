#include <stdio.h>
#include <yaml.h>
#include "config.h"


int config_parse_mapping(yaml_parser_t *parser, monikor_config_dict_t *dict) {
  yaml_event_t ev;
  monikor_config_dict_t *elm = NULL;

  while (42) {
    if (!yaml_parser_parse(parser, &ev))
      return 1;
    if (ev.type == YAML_MAPPING_END_EVENT) {
      yaml_event_delete(&ev);
      return 0;
    }
    switch(ev.type) {
      case YAML_SCALAR_EVENT:
        if (elm) {
          elm->next = monikor_config_dict_new();
          elm = elm->next;
        } else {
          elm = dict;
        }
        if (config_parse_key(parser, (char *)ev.data.scalar.value, elm)) {
          printf("bad return from parse_key\n");
          return 1;
        }
        break;

      default:
        printf("error in parse_mapping, event %d\n", ev.type);
        return 1;
        break;

    }
    yaml_event_delete(&ev);
  }
}
