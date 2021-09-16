#include "user/user.h"

int main(int argc, char *argv[]) {
  int primes[34];
  int num_primes = 34, pid;
  for (int i = 0; i < 34; i++) {
    primes[i] = i + 2;
  }

  while (num_primes > 0) {
    int p2c_pipe[2], c2p_pipe[2];
    pipe(p2c_pipe);
    pipe(c2p_pipe);
    pid = fork();
    if (pid < 0) {
      fprintf(2, "primes: fork() failed.\n");
      exit(1);
    } else if (pid > 0) {
      close(p2c_pipe[0]);
      close(c2p_pipe[1]);
      for (int i = 0; i < num_primes; i++) {
        write(p2c_pipe[1], (void*)&primes[i], sizeof(int));
      }
      close(p2c_pipe[1]);
      read(c2p_pipe[0], 0, 0);
      exit(0);
    } else {
      close(p2c_pipe[1]);
      close(c2p_pipe[0]);
      num_primes = -1;
      int actual_prime;
      int recieve_num;
      while (read(p2c_pipe[0], (void*)&recieve_num, sizeof(int)) > 0) {
        if (num_primes < 0) {
          num_primes++;
          actual_prime = recieve_num;
          printf("prime %d\n", actual_prime);
        } else {
          if (recieve_num % actual_prime != 0) {
            primes[num_primes++] = recieve_num;
          }
        }
      }
      close(p2c_pipe[0]);
    }
  }
  exit(0);
}
