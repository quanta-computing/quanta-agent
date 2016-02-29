#ifndef LOGGER_H_
# define LOGGER_H_

# include <syslog.h>
# include <stdarg.h>

# define MONIKOR_LOG_SEP ": "

# define MONIKOR_LOG_MIN_PRIO LOG_EMERG
# define MONIKOR_LOG_MAX_PRIO LOG_DEBUG
# define MONIKOR_LOG_DEFAULT -1

# define LOG_ERROR LOG_ERR

extern int monikor_log_level;


void monikor_logger_init(int prio, const char *file);
void monikor_logger_cleanup(void);


int monikor_vlog(int prio, const char *message, va_list ap);
int monikor_log(int prio, const char *message, ...);
int monikor_vlog_mod(int prio, const char *mod_name, const char *message, va_list ap);
int monikor_log_mod(int prio, const char *mod_name, const char *message, ...);


#endif /* end of include guard: LOGGER_H_ */
