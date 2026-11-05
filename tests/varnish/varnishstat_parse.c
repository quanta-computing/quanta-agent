// varnishstat-parse [file]: prints the metrics the varnish module extracts from a varnishstat -1 output
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "varnish.h"
#include "stubs.h"

static char *read_all(FILE *f) {
  size_t size = 0, cap = 65536;
  char *buf = malloc(cap);
  size_t n;

  if (!buf)
    return NULL;
  while ((n = fread(buf + size, 1, cap - size - 1, f)) > 0) {
    size += n;
    if (size + 1 >= cap) {
      char *bigger = realloc(buf, cap *= 2);
      if (!bigger) {
        free(buf);
        return NULL;
      }
      buf = bigger;
    }
  }
  buf[size] = 0;
  return buf;
}

int main(int argc, char **argv) {
  FILE *f = stdin;
  varnish_module_t mod = { .instance = NULL, .mon = NULL, .cmd_handler = NULL };
  char *output;

  if (argc > 2 || (argc == 2 && !strcmp(argv[1], "-h"))) {
    fprintf(stderr, "usage: %s [varnishstat-1-output-file]\n", argv[0]);
    return 2;
  }
  if (argc == 2 && !(f = fopen(argv[1], "r"))) {
    perror(argv[1]);
    return 1;
  }
  if (!(output = read_all(f))) {
    fprintf(stderr, "cannot read input\n");
    return 1;
  }
  varnish_fetch_metrics(&mod, output);
  for (size_t i = 0; i < recorded_metrics_count; i++)
    printf("%s %llu\n", recorded_metrics[i].name, (unsigned long long)recorded_metrics[i].value);
  free(output);
  return recorded_metrics_count ? 0 : 1;
}
