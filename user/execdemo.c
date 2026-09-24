#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int pid = fork();

  if (pid < 0) {
    fprintf(2, "execdemo: fork failed\n");
    exit(1);
  }

  if (pid == 0) { //子进程
    char *argv[] = {"ls", 0};

    exec("ls", argv);
    fprintf(2, "execdemo: exec ls failed\n");
    exit(1);
  }

  wait(0);
  printf("done\n");
  exit(0);
}
