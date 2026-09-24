#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

static void scan(int fd)
{
  char c;
  int n = 0, in = 0, valid = 1;
  while (read(fd, &c, 1) == 1) {
    if (c >= '0' && c <= '9') {
      if (!in) { in = 1; valid = 1; n = 0; }
      if (valid) n = n * 10 + c - '0';
    } else if (c == ' ' || c == '-' || c == '\r' || c == '\t' ||
               c == '\n' || c == '.' || c == '/' || c == ',') {
      if (in && valid && (n % 5 == 0 || n % 6 == 0)) printf("%d\n", n);
      in = 0;
    } else {
      if (!in) in = 1;
      valid = 0;
    }
  }
  if (in && valid && (n % 5 == 0 || n % 6 == 0)) printf("%d\n", n);
}

int main(int argc, char *argv[])
{
  int i, fd;
  if (argc == 1) { scan(0); exit(0); }
  for (i = 1; i < argc; i++) {
    fd = open(argv[i], O_RDONLY);
    if (fd < 0) { fprintf(2, "sixfive: cannot open %s\n", argv[i]); continue; }
    scan(fd); close(fd);
  }
  exit(0);
}
