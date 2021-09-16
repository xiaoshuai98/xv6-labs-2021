#include "user/user.h"

int main(int argc, char *argv[]) {
  int pid, message = 1;
  int p2c_pipe[2], c2p_pipe[2];

  pipe(p2c_pipe);
  pipe(c2p_pipe);
  pid = fork();
  if (pid < 0) {
    fprintf(2, "pingpong: fork() failed.\n");
    exit(1);
  } else if(pid == 0) {
    int recieve;
    close(p2c_pipe[1]);
    close(c2p_pipe[0]);
    read(p2c_pipe[0], (void*)&recieve, sizeof(int));
    printf("%d: received ping\n", getpid());
    write(c2p_pipe[1], (void*)&recieve, sizeof(int));
    close(p2c_pipe[0]);
    close(c2p_pipe[1]);
  } else {
    int recieve;
    close(p2c_pipe[0]);
    close(c2p_pipe[1]);
    write(p2c_pipe[1], (void*)&message, sizeof(int));
    read(c2p_pipe[0], (void*)&recieve, sizeof(int));
    printf("%d: received pong\n", getpid());
    close(p2c_pipe[1]);
    close(c2p_pipe[0]);
  }
  exit(0);
}
