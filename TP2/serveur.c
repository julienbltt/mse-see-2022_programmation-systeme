/* programme (c) M Kerrisk,
   adapté par P. Lalevée */
#include <signal.h>

#include "pse.h"

#define BUF_SIZE 1024

volatile int inputFd;

void abrt_handler(int signo) {
  printf("C'est CIAO !\n");
  close(inputFd);
  exit(0);
}

int main(int argc, char *argv[]) {
  char inputBuf[BUF_SIZE];
  FILE* pLogFile;

  signal(SIGABRT, abrt_handler);

  inputFd = open("fifo", O_RDONLY, 0644);
  pLogFile = fopen("journal.log", "a");

  while( strcmp(inputBuf, "fin") ) {
    lireLigne(inputFd, inputBuf);
    printf("[INPUT] > Message : %s\n", inputBuf);
    fprintf(pLogFile, "%s\n", inputBuf);
    printf("[INPUT] > Message write in log file.\n");
  }
  close(inputFd);
  fclose(pLogFile);

  exit(EXIT_SUCCESS);
}
