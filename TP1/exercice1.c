#include "pse.h"

extern char **environ;

int main(int argc, char *argv[]) {
  printf("argc = %d\n", argc);
  if (argc == 1) {
    erreur("%s: absence argument\n", argv[0]);
  }
  // BEGIN EXERCICE 1
  else {
    for(int i = 0 ; i < argc ; ++i) {
      printf("%s\n", argv[i]);
    }
  }
  // END EXERCICE 1
  exit(EXIT_SUCCESS);
}

