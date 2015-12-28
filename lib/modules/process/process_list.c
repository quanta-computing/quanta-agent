#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "process.h"

void process_metrics_init(process_metrics_t *metrics) {
  metrics->count = 0;
  metrics->interval.tv_sec = 0;
  metrics->interval.tv_usec = 0;
  metrics->cpu_user_usage = 0;
  metrics->cpu_kernel_usage = 0;
  metrics->mem_vsize = 0;
  metrics->mem_rss = 0;
}

process_t *process_new(const char *name, pid_t pid) {
  process_t *p;

  if (!(p = malloc(sizeof(*p))))
    return NULL;
  p->next = NULL;
  p->pid = pid;
  p->dirty = 0;
  if (!(p->name = strdup(name))) {
    free(p);
    return NULL;
  }
  process_metrics_init(&p->metrics);
  return p;
}

void process_free(process_t *p) {
  if (!p)
    return;
  free(p->name);
  free(p);
}

void process_dump(process_t *p) {
  printf("PROCESS %s [%u]: CPU %lu %lu\n", p->name, p->pid,
    p->metrics.cpu_user_usage, p->metrics.cpu_kernel_usage);
}

process_list_t *process_list_new(void) {
  process_list_t *l;

  if (!(l = malloc(sizeof(*l))))
    return NULL;
  l->first = NULL;
  l->last = NULL;
  l->size = 0;
  return l;
}

void process_list_free(process_list_t *list) {
  process_t *cur;
  process_t *next;

  if (!list)
    return;
  cur = list->first;
  while (cur) {
    next = cur->next;
    process_free(cur);
    cur = next;
  }
  free(list);
}

/*
** We keep the list sorted by name to aggregate metrics easily
*/
void process_list_push(process_list_t *list, process_t *process) {
  process_t *cur;

  if (!list->last) {
    list->first = list->last = process;
  } else {
    for (cur = list->first; cur->next && strcmp(cur->name, process->name); cur = cur->next);
    process->next = cur->next;
    cur->next = process;
    if (!process->next)
      list->last = process;
  }
  list->size++;
}
