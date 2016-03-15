#ifndef MONIKOR_CONFIG_H_
# define MONIKOR_CONFIG_H_

#include <stdio.h>
#include <yaml.h>
#include <stdint.h>
#include <sys/types.h>

#include "strl.h"

#define MONIKOR_DEFAULT_RUN_DIR "/"
#define MONIKOR_DEFAULT_PIDFILE "/tmp/monikor.pid"
#define MONIKOR_DEFAULT_CONFIG_PATH "/etc/monikor/monikor.yml"
#define MONIKOR_DEFAULT_MODULES_PATH "/usr/lib/monikor/modules"
#define MONIKOR_DEFAULT_MODULES_CONFIG_PATH "/etc/monikor/mod.d"
#define MONIKOR_DEFAULT_POLL_INTERVAL 60
#define MONIKOR_DEFAULT_UPDATE_INTERVAL 30
#define MONIKOR_DEFAULT_SERVER_TIMEOUT 5
#define MONIKOR_DEFAULT_SERVER_MAX_SEND_SIZE (5 * 1024 * 1024)
#define MONIKOR_DEFAULT_MAX_CACHE_SIZE (30 * 1024 * 1024)

#define MONIKOR_DEFAULT_LISTEN_MODE 0660

#define MONIKOR_QUANTA_TOKEN_MAX_LENGTH 42
#define MONIKOR_HOSTID_MAX_LENGTH 42

#define MONIKOR_CONFIG_EXT ".yml"


typedef enum {
  MONIKOR_CFG_DICT,
  MONIKOR_CFG_LIST,
  MONIKOR_CFG_SCALAR
} monikor_config_elem_type_t;


typedef struct monikor_config_dict_s {
  char *key;
  monikor_config_elem_type_t type;
  union {
    struct monikor_config_dict_s *dict;
    strl_t *list;
    char *value;
  } value;
  struct monikor_config_dict_s *next;
} monikor_config_dict_t;


typedef struct {
  monikor_config_dict_t *full_config;
  char *config_path;

  char *user;
  char *group;
  char *run_dir;
  char *pidfile;
  uint8_t daemonize;

  struct {
    char *path;
    char *config_path;
    strl_t *modules;
  } modules;

  char *hostid;
  char *quanta_token;

  struct {
    char *url;
    int timeout;
    size_t max_send_size;
    char *proxy_url;
  } server;

  struct {
    char *path;
    char *user;
    char *group;
    mode_t mode;
  } listen;

  struct {
    int level;
    char *file;
  } logger;

  struct {
    int max_size;
  } cache;

  int poll_interval;
  int update_interval;

} monikor_config_t;


monikor_config_t *monikor_config_new(void);
void monikor_config_free(monikor_config_t *config);
monikor_config_t *monikor_load_config(char *config_path);
monikor_config_dict_t *monikor_load_mod_config(char *mod_config_path, char *mod_name);
int monikor_setup_config(monikor_config_t *config);

monikor_config_dict_t *monikor_config_dict_new(void);
void monikor_config_dict_free(monikor_config_dict_t *dict);
char *monikor_config_dict_get_scalar(monikor_config_dict_t *dict, const char *key);
strl_t *monikor_config_dict_get_list(monikor_config_dict_t *dict, const char *key);
monikor_config_dict_t *monikor_config_dict_get_dict(monikor_config_dict_t *dict, const char *key);

int monikor_parse_config_file(FILE *fh, monikor_config_dict_t *cfg);
int config_parse_key(yaml_parser_t *parser, const char *key, monikor_config_dict_t *dict);
int config_parse_mapping(yaml_parser_t *parser, monikor_config_dict_t *dict);
int config_parse_list(yaml_parser_t *parser, strl_t *list);


#endif /* end of include guard: MONIKOR_CONFIG_H_ */
