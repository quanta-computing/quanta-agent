#ifndef MONIKOR_METRIC_H_
#define MONIKOR_METRIC_H_

# include <stddef.h>
# include <time.h>

# define MONIKOR_INT_SIZE 64


typedef enum {
  MONIKOR_INTEGER,
  MONIKOR_FLOAT,
  MONIKOR_STRING,
} monikor_metric_type_t;


typedef union {
  unsigned long _int:MONIKOR_INT_SIZE;
  float   _float;
  char    *_string;
} monikor_metric_value_t;


typedef struct {
  char                    *name;
  time_t                  clock;
  monikor_metric_type_t   type;
  monikor_metric_value_t  value;
} monikor_metric_t;


typedef struct _monikor_metric_list_node {
  struct              _monikor_metric_list_node *next;
  monikor_metric_t    *metric;
} monikor_metric_list_node_t;


typedef struct {
  size_t                      size;
  monikor_metric_list_node_t  *first;
  monikor_metric_list_node_t  *last;
} monikor_metric_list_t;


monikor_metric_t *monikor_metric_new(const char *name, time_t clock);
monikor_metric_t *monikor_metric_integer(const char *name, time_t clock, long value);
monikor_metric_t *monikor_metric_float(const char *name, time_t clock, float value);
monikor_metric_t *monikor_metric_string(const char *name, time_t clock, const char *value);
void monikor_metric_free(monikor_metric_t *metric);


monikor_metric_list_t *monikor_metric_list_new(void);
monikor_metric_list_node_t *monikor_metric_list_node_new(monikor_metric_t *metric);
void monikor_metric_list_free(monikor_metric_list_t *list);
void monikor_metric_list_node_free(monikor_metric_list_node_t *node);

int monikor_metric_list_push(monikor_metric_list_t *list, monikor_metric_t *metric);
int monikor_metric_list_push_node(monikor_metric_list_t *list, monikor_metric_list_node_t *metric);

void monikor_metric_list_apply(monikor_metric_list_t *list, void (*apply)(monikor_metric_t *));

void monikor_metric_list_concat(monikor_metric_list_t *head, monikor_metric_list_t *tail);

#endif /* end of include guard: MONIKOR_METRIC_H_ */
