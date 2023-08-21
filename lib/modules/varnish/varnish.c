#include <stdlib.h>
#include <errno.h>


#include "monikor.h"
#include "varnish.h"


static char *varnish_init_timeout(monikor_t *mon, monikor_config_dict_t *config) {
  char *timeout_s;

  timeout_s = monikor_config_dict_get_scalar(config, "varnish.varnishstat_timeout");
  if (timeout_s) {
    long value = atol(timeout_s);
    // We got no digit or invalid characters, log error and return default
    if (!value) {
      monikor_log_mod(LOG_WARNING, MOD_NAME,
        "Invalid value '%s' for varnishstat_timeout, fallbacking to default timeout (%s)\n",
        timeout_s, MONIKOR_VARNISH_DEFAULT_VARNISHSTAT_TIMEOUT_ARG
      );
      return MONIKOR_VARNISH_DEFAULT_VARNISHSTAT_TIMEOUT_ARG;
    } else if (value > mon->config->poll_interval) {
      monikor_log_mod(LOG_WARNING, MOD_NAME,
        "varnishstat_timeout cannot be greater than poll interval (%d), fallbacking to default timeout (%s)\n",
        mon->config->poll_interval, MONIKOR_VARNISH_DEFAULT_VARNISHSTAT_TIMEOUT_ARG
      );
      return MONIKOR_VARNISH_DEFAULT_VARNISHSTAT_TIMEOUT_ARG;
    }
    return timeout_s;
  } else {
    return MONIKOR_VARNISH_DEFAULT_VARNISHSTAT_TIMEOUT_ARG;
  }
}

void *varnish_init(monikor_t *mon, monikor_config_dict_t *config) {
  varnish_module_t *mod;

  if (!(mod = malloc(sizeof(*mod))))
    return NULL;
  mod->mon = mon;
  mod->cmd_handler = NULL;
  mod->varnishstat_path = monikor_config_dict_get_scalar(config, "varnish.varnishstat_path");
  if (!mod->varnishstat_path)
    mod->varnishstat_path = MONIKOR_VARNISH_DEFAULT_VARNISHSTAT_PATH;
  mod->varnishstat_timeout_arg = varnish_init_timeout(mon, config);
  mod->instance = monikor_config_dict_get_scalar(config, "varnish.instance");
  if (mod->instance && strlen(mod->instance) >= MONIKOR_VARNISH_MAX_INSTANCE_LENGTH) {
    mod->instance[MONIKOR_VARNISH_MAX_INSTANCE_LENGTH - 1] = 0;
    monikor_log_mod(LOG_WARNING, MOD_NAME, "Instance name too long, stripped to %d chars\n",
      MONIKOR_VARNISH_MAX_INSTANCE_LENGTH);
  }

  return mod;
}

void varnish_cleanup(monikor_t *mon, void *data) {
  varnish_module_t *mod = (varnish_module_t *)data;

  if (mod->cmd_handler) {
    monikor_reap_process(mon, mod->cmd_handler->pid);
    monikor_command_unregister_io_handlers(mon, mod->cmd_handler);
    monikor_command_free(mod->cmd_handler);
  }
  free(mod);
}

static size_t init_varnishstat_argv(varnish_module_t *mod, char *argv[]) {
  size_t argc = 0;

  argv[argc++] = mod->varnishstat_path;
  argv[argc++] = "-1";
  argv[argc++] = "-t";
  argv[argc++] = mod->varnishstat_timeout_arg;
  if (mod->instance) {
    argv[argc++] = "-n";
    argv[argc++] = mod->instance;
  }
  argv[argc] = NULL;
  return argc;
}

int varnish_poll(monikor_t *mon, void *data) {
  varnish_module_t *mod = (varnish_module_t *)data;
  char *argv[MONIKOR_VARNISH_VARNISHSTAT_MAX_ARGS + 1];

  if (mod->cmd_handler) {
    command_exec_t *cmd = (command_exec_t *)mod->cmd_handler->data;
    if (!monikor_command_finished(cmd->state)) {
      monikor_log_mod(LOG_WARNING, MOD_NAME,
        "previous varnishstat execution did not finished, reaping process %d\n", mod->cmd_handler->pid);
      monikor_reap_process(mon, mod->cmd_handler->pid);
    }
    monikor_command_unregister_io_handlers(mon, mod->cmd_handler);
    monikor_command_free(mod->cmd_handler);
    mod->cmd_handler = NULL;
  }
  init_varnishstat_argv(mod, argv);
  mod->cmd_handler = monikor_command_exec(mod->varnishstat_path, argv,
    &varnish_poll_metrics, (void *)mod
  );
  if (!mod->cmd_handler) {
    monikor_log_mod(LOG_ERR, MOD_NAME, "cannot execute varnishstat: %s\n", strerror(errno));
    return -1;
  }
  monikor_log_mod(LOG_DEBUG, MOD_NAME,
    "Forked varnishstat with pid %d\n",
    ((command_exec_t *)mod->cmd_handler->data)->pid);
  monikor_command_register_io_handlers(mon, mod->cmd_handler);

  return MONIKOR_MOD_DEFERRED_METRICS_CODE;
}
