#ifndef MONIKOR_METRIC_H_
#define MONIKOR_METRIC_H_

# include <stddef.h>
# include <stdint.h>
# include <sys/time.h>

typedef enum {
  MONIKOR_INTEGER,
  MONIKOR_FLOAT,
  MONIKOR_STRING,
} monikor_metric_type_t;

typedef union {
  uint64_t _int;
  float   _float;
  char    *_string;
} monikor_metric_value_t;

# define MONIKOR_METRIC_DELTA       (1 << 0)
# define MONIKOR_METRIC_TIMEDELTA   (MONIKOR_METRIC_DELTA | (1 << 1))
# define MONIKOR_METRIC_AGGREGATE   (1 << 2)

typedef struct {
  char                    *name;
  uint16_t                id;
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
  size_t                      data_size;
  monikor_metric_list_node_t  *first;
  monikor_metric_list_node_t  *last;
} monikor_metric_list_t;


typedef struct {
  monikor_metric_list_t *current;
  monikor_metric_list_t *delta;
  monikor_metric_list_t *cache;
} monikor_metric_store_t;


/*
** These 2 structs are here for reference and/or internal usage but should not be read/write directly
** in the serialized binary data because of padding and endianess stuff. You must properly encode or
** decode the fields in the same order with the correct endianess.
*/
typedef struct {
  uint64_t clock_sec;
  uint64_t clock_usec;
  uint16_t name_size;
  uint16_t data_size;
  uint8_t type;
  uint8_t flags;
} monikor_serialized_metric_hdr_t;

#define METRIC_HDR_CLOCK_SEC_OFF (0)
#define METRIC_HDR_CLOCK_USEC_OFF (sizeof(uint64_t))
#define METRIC_HDR_NAME_SIZE_OFF (2 * sizeof(uint64_t))
#define METRIC_HDR_DATA_SIZE_OFF (2 * sizeof(uint64_t) + sizeof(uint16_t))
#define METRIC_HDR_TYPE_OFF (2 * sizeof(uint64_t) + 2 * sizeof(uint16_t))
#define METRIC_HDR_FLAGS_OFF (2 * sizeof(uint64_t) + 2 * sizeof(uint16_t) + sizeof(uint8_t))
#define SERIALIZED_METRIC_HDR_SIZE (2 * sizeof(uint64_t) + 2 * sizeof(uint16_t) + 2 * sizeof(uint8_t))

typedef struct {
  uint8_t version;
  uint16_t count;
  uint16_t data_size;
} monikor_serialized_metric_list_hdr_t;

#define METRIC_SERIALIZER_VERSION 1
#define METRIC_LIST_HDR_VERSION_OFF (0)
#define METRIC_LIST_HDR_COUNT_OFF (sizeof(uint8_t))
#define METRIC_LIST_HDR_DATA_SIZE_OFF (sizeof(uint8_t) + sizeof(uint16_t))
#define SERIALIZED_METRIC_LIST_HDR_SIZE (sizeof(uint8_t) + 2 * sizeof(uint16_t))


// Metrics
monikor_metric_t *monikor_metric_new(const char *name, const struct timeval *clock);
monikor_metric_t *monikor_metric_clone(const monikor_metric_t *src);
void monikor_metric_free(monikor_metric_t *metric);

monikor_metric_t *monikor_metric_integer(const char *name, const struct timeval *clock,
  uint64_t value, uint8_t flags);
monikor_metric_t *monikor_metric_float(const char *name, const struct timeval *clock,
  float value, uint8_t flags);
monikor_metric_t *monikor_metric_integer_id(const char *name, const struct timeval *clock,
  uint64_t value, uint8_t flags, uint16_t id);
monikor_metric_t *monikor_metric_float_id(const char *name, const struct timeval *clock,
  float value, uint8_t flags, uint16_t id);
monikor_metric_t *monikor_metric_string(const char *name, const struct timeval *clock,
  const char *value);

monikor_metric_t *monikor_metric_compute_delta(monikor_metric_t *a, monikor_metric_t *b);
int monikor_metric_add(monikor_metric_t *dst, const monikor_metric_t *src);

size_t monikor_metric_data_size(const monikor_metric_t *metric);

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

size_t monikor_metric_list_remove_if(monikor_metric_list_t *list,
  int (*f)(monikor_metric_t *, void *), void *data);

int monikor_metric_list_serialize(const monikor_metric_list_t *metrics, void **data, size_t *size);
void monikor_metric_list_header_unserialize(void *data,
  monikor_serialized_metric_list_hdr_t *hdr);
size_t monikor_metric_list_unserialize(void *data, monikor_serialized_metric_list_hdr_t *hdr,
  monikor_metric_list_t **metrics);


// Metric store
monikor_metric_store_t *monikor_metric_store_new(void);
void monikor_metric_store_free(monikor_metric_store_t *store);

int monikor_metric_store_push_delta(monikor_metric_store_t *store, monikor_metric_t *metric);
int monikor_metric_store_push(monikor_metric_store_t *store, monikor_metric_t *metric);
int monikor_metric_store_lpush(monikor_metric_store_t *store, monikor_metric_list_t *metrics);
void monikor_metric_store_flush(monikor_metric_store_t *store);
void monikor_metric_store_cache(monikor_metric_store_t *store);
void monikor_metric_store_flush_all(monikor_metric_store_t *store);
size_t monikor_metric_store_evict_cache(monikor_metric_store_t *store, size_t max_size);
size_t monikor_metric_store_evict_delta(monikor_metric_store_t *store, struct timeval *clock);

// Utils
#ifndef htonll
uint64_t htonll(uint64_t hostlong);
uint64_t ntohll(uint64_t netlong);
#endif

float ntohf(uint32_t netfloat);
uint32_t htonf(float hostfloat);


#endif /* end of include guard: MONIKOR_METRIC_H_ */
