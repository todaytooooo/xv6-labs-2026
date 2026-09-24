#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

static char *exec_argv[16];
static int exec_argc;

static void run_exec(char *path)
{
  int pid;
  exec_argv[exec_argc] = path;
  exec_argv[exec_argc + 1] = 0;
  if ((pid = fork()) == 0) {
    exec(exec_argv[0], exec_argv);
    fprintf(2, "find: exec %s failed\n", exec_argv[0]);
    exit(1);
  }
  if (pid > 0) wait(0);
}

static void find(char *path, char *name)
{
  char buf[512], *p, *base;
  int fd;
  struct dirent de;
  struct stat st;
  if ((fd = open(path, O_RDONLY)) < 0) { fprintf(2, "find: cannot open %s\n", path); return; }
  if (fstat(fd, &st) < 0) { fprintf(2, "find: cannot stat %s\n", path); close(fd); return; }
  p = path + strlen(path);
  while (p > path && p[-1] == '/') p--;
  base = p;
  while (base > path && base[-1] != '/') base--;
  if (strcmp(base, name) == 0) {
    if (exec_argc) run_exec(path); else printf("%s\n", path);
  }
  if (st.type != T_DIR) { close(fd); return; }
  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) { fprintf(2, "find: path too long\n"); close(fd); return; }
  strcpy(buf, path); p = buf + strlen(buf); *p++ = '/';
  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0) continue;
    if (de.name[0] == '.' && (de.name[1] == 0 || (de.name[1] == '.' && de.name[2] == 0))) continue;
    memmove(p, de.name, DIRSIZ); p[DIRSIZ] = 0; find(buf, name);
  }
  close(fd);
}

int main(int argc, char *argv[])
{
  int i, exec_at = -1;
  if (argc < 3) { fprintf(2, "usage: find path name [-exec cmd [args...]]\n"); exit(1); }
  for (i = 3; i < argc; i++) if (strcmp(argv[i], "-exec") == 0) { exec_at = i + 1; break; }
  if (exec_at >= 0) {
    for (i = exec_at; i < argc && exec_argc < 14; i++) exec_argv[exec_argc++] = argv[i];
    if (exec_argc == 0) { fprintf(2, "find: missing command after -exec\n"); exit(1); }
  }
  find(argv[1], argv[2]);
  exit(0);
}
