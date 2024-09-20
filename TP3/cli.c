#include "pse.h"

#define CMD   "client"

int main(int argc, char *argv[]) {
  int sock, ret;
  struct sockaddr_in *adrServ;
  char ligne[LIGNE_MAX];
  int lgEcr;



  printf("ligne> ");
  if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
    printf("arret par CTRL-D\n");

  else {
    lgEcr = ecrireLigne(sock, ligne);
    if (lgEcr == -1)
      erreur_IO("ecrireLigne");
  
    printf("%s: %d octets envoyes\n", CMD, lgEcr);
  }

  if (close(sock) == -1)
    erreur_IO("close");

  exit(EXIT_SUCCESS);
}
