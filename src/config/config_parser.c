#include <stdio.h>
#include <yaml.h>
#include "config.h"


int monikor_parse_config_file(FILE *fh, monikor_config_dict_t *cfg) {
  yaml_parser_t parser;
  yaml_event_t ev;

  if (!yaml_parser_initialize(&parser))
    return 1;
  yaml_parser_set_input_file(&parser, fh);
  while (42) {
    if (!yaml_parser_parse(&parser, &ev))
      return 1;
    if (ev.type == YAML_STREAM_END_EVENT) {
      yaml_event_delete(&ev);
      break;
    }
    switch (ev.type) {
      case YAML_STREAM_START_EVENT:
        break;

      case YAML_DOCUMENT_START_EVENT:
        break;

      case YAML_DOCUMENT_END_EVENT:
        break;

      case YAML_MAPPING_START_EVENT:
        if (config_parse_mapping(&parser, cfg))
          return 1;
        break;

      default:
        printf("Error, caught event %d\n", ev.type);
        yaml_event_delete(&ev);
        return 1;
        break;
    }
    yaml_event_delete(&ev);
  }
  yaml_parser_delete(&parser);
  return 0;
}
