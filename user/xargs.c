#include "user/user.h"

void printArgs(int argc, char *args[]) {
  for (int i = 0; i < argc; i++) {
    printf("%s\n", args[i]);
  }
}

int main(int argc, char *argv[]) {
  char buf[512];
  char *args[argc + 1];
  int i = 0, pid;

  memset((void*)buf, '\0', 512);
  memset((void*)args, 0, argc + 2);
  for (int i = 1; i < argc; i++) {
    args[i - 1] = argv[i];
  }
  
  while (read(0, (void*)(buf + i), 1) > 0) {
    if (buf[i] == '\n') {
      buf[i] = '\0';
      args[argc - 1] = buf;
      pid = fork();
      if (pid < 0) {
        fprintf(2, "xargs: fork() failed.\n");
        exit(1);
      } else if (pid == 0) {
        if (exec(argv[1], args) < 0) {
          fprintf(2, "xargs: exec() failed.\n");
          exit(1);
        }
      } else {
        wait(0);
      }
      i = 0;
    } else {
      i++;
    }
  }
  exit(0);
}
