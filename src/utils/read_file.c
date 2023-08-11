#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define MNK_READ_FILE_RD_SZ 4096

char *monikor_read_fd(int fd) {
  char *content = NULL;
  size_t content_size = 0;
  ssize_t read_bytes;

  do {
    if (!(content = realloc(content, content_size + MNK_READ_FILE_RD_SZ + 1)))
      goto err;
    read_bytes = read(fd, content + content_size, MNK_READ_FILE_RD_SZ);
    if (read_bytes == -1)
      goto err;
    content_size += read_bytes;
    content[content_size] = 0;
  } while (read_bytes == MNK_READ_FILE_RD_SZ);
  return content;

err:
  free(content);
  return NULL;
}

char *monikor_read_file(const char *filepath) {
  int fd;
  char *content;

  if ((fd = open(filepath, O_RDONLY | O_CLOEXEC)) == -1)
    return NULL;
  content = monikor_read_fd(fd);
  close(fd);
  return content;
}
