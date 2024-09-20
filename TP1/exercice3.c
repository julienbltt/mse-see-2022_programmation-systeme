#include "pse.h"

int main(int argc, char *argv[]) {
  /* requete DNS */
  int code;
  struct addrinfo *infos, hints;
  struct sockaddr_in *adresse;
  
  int id_port = 0;

  memset(&hints, 0, sizeof(struct addrinfo));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  
  code = getaddrinfo(argv[1], argv[2], &hints, &infos); // Nom de domaine et le service passés en argument.
  if (code != 0) {
    fprintf(stderr, "Erreur: %s\n", gai_strerror(code));
    exit(EXIT_FAILURE);
  }

  adresse = (struct sockaddr_in *) infos->ai_addr;

  /* adresse->sin_addr.s_addr doit contenir 0xC131AEC2 ie 193.49.174.194 */  
  printf("ADRESSE IP : %s[%X]\n", inet_ntoa(adresse->sin_addr), ntohl(adresse->sin_addr.s_addr) ); // Affichage de l'adresse IP du nom de domaine.
  /* adresse->sin_port doit contenir 80 */
  id_port = ntohs(adresse->sin_port);
  printf("PORT : %d[%hX]\n", id_port, id_port ); // Affichage du port du service ciblé.

  freeaddrinfo(infos);
  exit(EXIT_SUCCESS);
}

