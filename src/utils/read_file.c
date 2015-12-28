#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define MNK_READ_FILE_RD_SZ 4096

char *monikor_read_file(const char *filepath) {
  char *content = NULL;
  size_t content_size = 0;
  ssize_t read_bytes;
  int fd;

  if ((fd = open(filepath, O_RDONLY)) == -1)
    goto err;
  do {
    if (!(content = realloc(content, content_size + MNK_READ_FILE_RD_SZ + 1)))
      goto err;
    read_bytes = read(fd, content + content_size, MNK_READ_FILE_RD_SZ);
    if (read_bytes == -1)
      goto err;
    content_size += read_bytes;
    content[content_size] = 0;
  } while (read_bytes == MNK_READ_FILE_RD_SZ);
  close(fd);
  return content;

err:
  if (fd != -1)
    close(fd);
  free(content);
  return NULL;
}
