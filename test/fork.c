#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

static int usage(void) {
  fprintf(stderr, "Usage: fork <nb_fork>\n");
  return 1;
}

unsigned fact(unsigned n) {
  if (!n)
    return 1;
  return n * fact(n - 1);
}

void run(void) {
  unsigned sum = 0;

  while (42) {
    for (int i = 0; i < 100; i++) {
      sum += fact(i);
      // fprintf(stderr, "FACT %d = %u\n", i, fact(i));
    }
    usleep(10);
  }
}

int main(int argc, char **argv) {
  int nb;
  pid_t pid;

  if (argc != 2)
    return usage();
  for (nb = atoi(argv[1]); nb; nb--) {
    pid = fork();
    if (pid == -1)
      perror("cannot fork");
    else if (pid == 0)
      run();
  }
  while (42)
    sleep(30);
  return 0;
}
