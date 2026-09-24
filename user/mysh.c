#include "kernel/types.h"
#include "user/user.h"

#define INPUT_SIZE 128
#define MAXARGS 16

// Read one line a byte at a time.  Return -1 on EOF before any input and
// -2 when the line does not fit in the input buffer.
static int
readline(char *buf, int size)
{
  int n = 0;
  int overflow = 0;
  char c;

  for (;;) {
    int result = read(0, &c, 1);

    if (result < 0)
      return -1;
    if (result == 0) {
      if (n == 0)
        return -1;
      break;
    }
    if (c == '\n' || c == '\r')
      break;
    if (n < size - 1)
      buf[n++] = c;
    else
      overflow = 1;
  }

  buf[n] = 0;
  return overflow ? -2 : n;
}

// Split the command in place on spaces and tabs.
static int
makeargv(char *line, char **argv, int maxargs)
{
  int argc = 0;
  char *p = line;

  for (;;) {
    while (*p == ' ' || *p == '\t')
      p++;
    if (*p == 0)
      break;
    if (argc == maxargs - 1)
      return -1;

    argv[argc++] = p;
    while (*p != 0 && *p != ' ' && *p != '\t')
      p++;
    if (*p != 0)
      *p++ = 0;
  }

  argv[argc] = 0;
  return argc;
}

int
main(void)
{
  char line[INPUT_SIZE];
  char *argv[MAXARGS];

  for (;;) {
    int length;
    int argc;
    int pid;

    write(1, "> ", 2);
    length = readline(line, sizeof(line));
    if (length == -1)
      break;
    if (length == -2) {
      fprintf(2, "mysh: input line too long\n");
      continue;
    }

    argc = makeargv(line, argv, MAXARGS);
    if (argc < 0) {
      fprintf(2, "mysh: too many arguments\n");
      continue;
    }
    if (argc == 0) //空行处理
      continue;
    if (argc == 1 && strcmp(argv[0], "exit") == 0)
      break;

    pid = fork();//不是退出就创建子进程，调用exec执行相应的命令
    if (pid < 0) {
      fprintf(2, "mysh: fork failed\n");
      continue;
    }
    if (pid == 0) {
      exec(argv[0], argv); // 失败时才返回，成功了直接子进程结束了，
      /*exec()
  它会替换当前进程的：

  - 程序代码；
  - 全局变量和其他数据；
  - 栈；
  - 堆。

      */
      fprintf(2, "mysh: exec %s failed\n", argv[0]);
      exit(1);
    }
    wait(0);
  }

  exit(0);
}
