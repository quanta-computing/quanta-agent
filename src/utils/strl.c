#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "strl.h"

strl_t *strl_new(void) {
  strl_t *l;

  if (!(l = malloc(sizeof(*l))))
    return NULL;
  l->size = 0;
  l->first = NULL;
  l->last = NULL;
  return l;
}

void strl_delete(strl_t *l) {
  strl_node_t *it;
  strl_node_t *next = NULL;

  if (!l)
    return;
  for (it = l->first; it; it = next) {
    next = it->next;
    strl_node_delete(it);
  }
  free(l);
}

strl_node_t *strl_node_new(char *s) {
  strl_node_t *n;

  if (!(n = malloc(sizeof(*n))) || !(n->str = strdup(s))) {
    free(n);
    return NULL;
  }
  n->next = NULL;
  return n;
}

void strl_node_delete(strl_node_t *n) {
  if (!n)
    return;
  free(n->str);
  free(n);
}

int strl_push(strl_t *l, char *s) {
  strl_node_t *new;

  if (!(new = strl_node_new(s)))
    return -1;
  if (!l->size)
    l->first = new;
  else
    l->last->next = new;
  l->last = new;
  l->size++;
  return 0;
}

int strl_vpush_many(strl_t *l, va_list ap) {
  char *s;
  int n = 0;

  while ((s = va_arg(ap, char *))) {
    if (!(strl_push(l, s)))
      n++;
  }
  return n;
}

int strl_push_many(strl_t *l, ...) {
  va_list ap;
  int ret;

  va_start(ap, l);
  ret = strl_vpush_many(l, ap);
  va_end(ap);
  return ret;
}

/*
** This function appends 2 lists
** Please note that tail contents are definitely moved to head, so tail is an empty list after
** the call to strl_concat and can be safely freed with a call to
** metric_list_delete
*/
void strl_concat(strl_t *head, strl_t *tail) {
  if (head->size && tail->size) {
    head->last->next = tail->first;
    head->last = tail->last;
  } else if (tail->size) {
    head->first = tail->first;
    head->last = tail->last;
  }
  head->size += tail->size;
  tail->size = 0;
  tail->first = NULL;
  tail->last = NULL;
}

void strl_apply(strl_t *l, void (*apply)(char *s)) {
  for (strl_node_t *it = l->first; it; it = it->next)
    apply(it->str);
}

void strl_apply_data(strl_t *l, void (*apply)(char *s, void *data), void *data) {
  for (strl_node_t *it = l->first; it; it = it->next)
    apply(it->str, data);
}

int strl_contains(strl_t *l, const char *s) {
  for (strl_node_t *it = l->first; it; it = it->next) {
    if (!strcmp(it->str, s))
      return 1;
  }
  return 0;
}

strl_t *strl_from_separated_string(const char *sc, const char *sep) {
  strl_t *l;
  char *last;
  char *s;

  if (!(s = strdup(sc)) || !(l = strl_new())) {
    free(s);
    return NULL;
  }
  for (char *str = strtok_r(s, sep, &last); str; str = strtok_r(NULL, sep, &last)) {
    if (strl_push(l, str)) {
      free(s);
      strl_delete(l);
      return NULL;
    }
  }
  free(s);
  return l;
}
