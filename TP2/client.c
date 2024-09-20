/* programme (c) M Kerrisk,
   adapté par P. Lalevée */
#include <signal.h>
#include <string.h>

#include "pse.h"

#define BUF_SIZE 1024

volatile int outputFd;

void abrt_handler(int signo) {
  close(outputFd);
  exit(0);
}

int main(int argc, char *argv[]) {
  char outputBuf[BUF_SIZE];

  signal(SIGABRT, abrt_handler);

  outputFd = open("fifo", O_WRONLY, 0644);
  do {
    printf("Entrez un message : ");
    fgets(outputBuf, BUF_SIZE, stdin);
    ecrireLigne(outputFd, outputBuf);
  } while (strcmp(outputBuf, "fin\n") != 0);
  
  close(outputFd);

  exit(EXIT_SUCCESS);
}
