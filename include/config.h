#ifndef MONIKOR_CONFIG_H_
# define MONIKOR_CONFIG_H_

#include <stdio.h>
#include <yaml.h>

#define MONIKOR_DEFAULT_CONFIG_PATH "/etc/monikor/monikor.yml"


typedef enum {
  DICT,
  LIST,
  SCALAR
} monikor_config_elem_type_t;


typedef struct monikor_config_dict_s {
  char *key;
  monikor_config_elem_type_t type;
  union {
    struct monikor_config_dict_s *dict;
    // monikor_str_list_t *list;
    char *value;
  } value;
  struct monikor_config_dict_s *next;
} monikor_config_dict_t;


typedef struct {
  monikor_config_dict_t *full_config;
  char *config_path;
  char *modules_path;
  char *sock_path;
  int log_level;
} monikor_config_t;


monikor_config_t *monikor_config_new(void);
void monikor_config_free(monikor_config_t *config);
monikor_config_t *monikor_load_config(const char *config_path);

monikor_config_dict_t *monikor_config_dict_new(void);
void monikor_config_dict_free(monikor_config_dict_t *dict);
char *monikor_config_get_scalar(const monikor_config_t *config, const char *key);
char *monikor_config_dict_get_scalar(const monikor_config_dict_t *dict, const char *key);

int monikor_parse_config_file(FILE *fh, monikor_config_t *cfg);
int config_parse_key(yaml_parser_t *parser, const char *key, monikor_config_dict_t *dict);
int config_parse_mapping(yaml_parser_t *parser, monikor_config_dict_t *dict);

#endif /* end of include guard: MONIKOR_CONFIG_H_ */
