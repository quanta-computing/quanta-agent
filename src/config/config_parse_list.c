#include <yaml.h>

#include "config.h"
#include "strl.h"


int config_parse_list(yaml_parser_t *parser, strl_t *list) {
  yaml_event_t ev;

  while (42) {
    if (!yaml_parser_parse(parser, &ev))
      return 1;
    if (ev.type == YAML_SEQUENCE_END_EVENT) {
      yaml_event_delete(&ev);
      return 0;
    }
    switch(ev.type) {
      case YAML_SCALAR_EVENT:
        if (strl_push(list, (char *)ev.data.scalar.value)) {
          printf("error in parse_list: cannot add elem to list\n");
          return 1;
        }
        break;

      default:
        printf("error in parse_list, event %d\n", ev.type);
        return 1;
        break;

    }
    yaml_event_delete(&ev);
  }

}
