#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

const char* fmtname(const char *path) {
  const char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  return p;
}

void find(const char *cur_path, const char *target) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  memset((void*)buf, '\0', 512);
  if((fd = open(cur_path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", cur_path);
    return;
  }
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", cur_path);
    close(fd);
    return;
  }

  switch (st.type) {
    case T_FILE: {
      if (!strcmp(fmtname(cur_path), target)) {
        printf("%s\n", cur_path);
      }
      break;
    }
    case T_DIR: {
      strcpy(buf, cur_path);
      p = buf+strlen(buf);
      *p++ = '/';
      while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0 || !strcmp(".", de.name) || !strcmp("..", de.name))
          continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        find(buf, target);
      }
      break;
    }
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(2, "Usage: find directory file...\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  //printf("%d\n", strcmp(argv[1], argv[2]));
  exit(0);
}
