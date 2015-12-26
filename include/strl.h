#ifndef STRL_H_
#define STRL_H_

#include <stdarg.h>

typedef struct strl_node_s {
  char *str;
  struct strl_node_s *next;
} strl_node_t;

typedef struct {
  size_t size;
  strl_node_t *first;
  strl_node_t *last;
} strl_t;


strl_t *strl_new(void);
void strl_delete(strl_t *l);

strl_node_t *strl_node_new(char *s);
void strl_node_delete(strl_node_t *n);

int strl_push(strl_t *l, char *s);
int strl_vpush_many(strl_t *l, va_list ap);
int strl_push_many(strl_t *l, ...);
void strl_concat(strl_t *head, strl_t *tail);

void strl_apply(strl_t *l, void (*apply)(char *s));
void strl_apply_data(strl_t *l, void (*apply)(char *s, void *data), void *data);

strl_t *strl_from_separated_string(const char *s, const char *sep);

#endif /* end of include guard: STRL_H_ */
