#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int pid = fork();

  if (pid < 0) {
    fprintf(2, "forkdemo: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    printf("child: pid=%d\n", getpid());
    exit(0);
  }

  printf("parent: pid=%d, child=%d\n", getpid(), pid);
  wait(0);// 父进程在等待子进程结束之前就已经打印完了，所以说打印结果是不可预料的 
  /* eg.
  parent: pcihdi=ld6:,  pcihdi=ld7=
  7

  parent: pid=3, child=4
  child: pid=4
  */
  exit(0);
}
