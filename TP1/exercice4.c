#include "pse.h"

int main(int argc, char *argv[]) {
  struct sockaddr_in *adresse;
  
  if(argc == 1) {
    erreur("%s: absence argument\n", argv[0]);
  }
  else if(argc < 3) {
    erreur("%s <name> <service> : manque argument\n", argv[0]);
  }
  else {
    adresse = resolv(argv[1], argv[2]);
    printf("ADRESSE IP : %s\n", stringIP(adresse->sin_addr.s_addr) );
    printf("PORT : %d\n", ntohs(adresse->sin_port) );
  }
  exit(EXIT_SUCCESS);
}

