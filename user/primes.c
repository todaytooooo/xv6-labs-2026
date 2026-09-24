#include "kernel/types.h"
#include "user/user.h"

static void filter(int in);
static void (*next_filter)(int) = filter;

static void filter(int in)
{
  int p, n, fd[2];
  if (read(in, &p, sizeof(p)) != sizeof(p)) { close(in); exit(0); }
  printf("prime %d\n", p);
  if (pipe(fd) < 0) exit(1);
  if (fork() == 0) { close(in); close(fd[1]); next_filter(fd[0]); }
  close(fd[0]);
  while (read(in, &n, sizeof(n)) == sizeof(n))
    if (n % p != 0) write(fd[1], &n, sizeof(n));
  close(in); close(fd[1]); wait(0); exit(0);
}

int main(void)
{
  int fd[2], i;
  if (pipe(fd) < 0) exit(1);
  if (fork() == 0) { close(fd[1]); filter(fd[0]); }
  close(fd[0]);
  for (i = 2; i <= 35; i++) write(fd[1], &i, sizeof(i));
  close(fd[1]); wait(0); exit(0);
}
