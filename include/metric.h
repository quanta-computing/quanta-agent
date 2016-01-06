#ifndef MONIKOR_METRIC_H_
#define MONIKOR_METRIC_H_

# include <stddef.h>
# include <stdint.h>
# include <sys/time.h>

# define MONIKOR_INT_SIZE 64


typedef enum {
  MONIKOR_INTEGER,
  MONIKOR_FLOAT,
  MONIKOR_STRING,
} monikor_metric_type_t;


# define MONIKOR_METRIC_DELTA       (1 << 0)
# define MONIKOR_METRIC_TIMEDELTA   (MONIKOR_METRIC_DELTA | (1 << 1))
# define MONIKOR_METRIC_AGGREGATE   (1 << 2)

typedef union {
  uint64_t _int;
  float   _float;
  char    *_string;
} monikor_metric_value_t;


typedef struct {
  char                    *name;
  struct timeval          clock;
  monikor_metric_type_t   type;
  monikor_metric_value_t  value;
  uint8_t                 flags;
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


typedef struct {
  monikor_metric_list_t *current;
  monikor_metric_list_t *delta;
  monikor_metric_list_t *cache;
} monikor_metric_store_t;


// Metrics
monikor_metric_t *monikor_metric_new(const char *name, const struct timeval *clock);
void monikor_metric_free(monikor_metric_t *metric);

monikor_metric_t *monikor_metric_integer(const char *name, const struct timeval *clock,
  uint64_t value, uint8_t flags);
monikor_metric_t *monikor_metric_float(const char *name, const struct timeval *clock,
  float value, uint8_t flags);
monikor_metric_t *monikor_metric_string(const char *name, const struct timeval *clock,
  const char *value);

monikor_metric_t *monikor_metric_compute_delta(monikor_metric_t *a, monikor_metric_t *b);

// Metric list
void monikor_metric_list_init(monikor_metric_list_t *list);
monikor_metric_list_t *monikor_metric_list_new(void);
monikor_metric_list_node_t *monikor_metric_list_node_new(monikor_metric_t *metric);
void monikor_metric_list_empty(monikor_metric_list_t *list);
void monikor_metric_list_free(monikor_metric_list_t *list);
void monikor_metric_list_node_free(monikor_metric_list_node_t *node);

int monikor_metric_list_push(monikor_metric_list_t *list, monikor_metric_t *metric);
int monikor_metric_list_push_node(monikor_metric_list_t *list, monikor_metric_list_node_t *metric);
monikor_metric_t *monikor_metric_list_pop(monikor_metric_list_t *list);

void monikor_metric_list_apply(monikor_metric_list_t *list, void (*apply)(monikor_metric_t *));

void monikor_metric_list_concat(monikor_metric_list_t *head, monikor_metric_list_t *tail);


// Metric store
monikor_metric_store_t *monikor_metric_store_new(void);
void monikor_metric_store_free(monikor_metric_store_t *store);

int monikor_metric_store_push_delta(monikor_metric_store_t *store, monikor_metric_t *metric);
int monikor_metric_store_push(monikor_metric_store_t *store, monikor_metric_t *metric);
int monikor_metric_store_lpush(monikor_metric_store_t *store, monikor_metric_list_t *metrics);
void monikor_metric_store_flush(monikor_metric_store_t *store);
void monikor_metric_store_cache(monikor_metric_store_t *store);

#endif /* end of include guard: MONIKOR_METRIC_H_ */
