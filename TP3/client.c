// client.c
#include "pse.h"

#define CMD   "client"
#define NOM_FIFO    "fifo"

int main(int argc, char *argv[]) {
  int sock, ret;
  struct sockaddr_in *adrServ;
  int fin = FAUX;
  char ligne[LIGNE_MAX];
  int lgEcr;

  // lorsque le client ecrit dans le fifo alors que le serveur a ete arrete,
  // il recoit le signal SIGPIPE qui par defaut met fin brutalement a
  // l'execution sans message d'erreur;
  // avec l'instruction qui suit on ignore ce signal, ecrireLigne sortira
  // en echec et le client affichera un message d'erreur
  signal(SIGPIPE, SIG_IGN);

  // Traitement des arguments
  if (argc != 3)
    erreur("usage: %s machine port\n", argv[0]);

  // Création du socket client
  printf("%s: creating a socket\n", CMD);
  sock = socket (AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    erreur_IO("socket");

  // Résolution de l'adresse de connexion
  printf("%s: DNS resolving for %s, port %s\n", CMD, argv[1], argv[2]);
  adrServ = resolv(argv[1], argv[2]);
  if (adrServ == NULL)
    erreur("adresse %s port %s inconnus\n", argv[1], argv[2]);

  printf("%s: adr %s, port %hu\n", CMD,
	       stringIP(ntohl(adrServ->sin_addr.s_addr)),
	       ntohs(adrServ->sin_port)); // Affihe l'adresse et le port du serveur

  // Connexion au serveur
  printf("%s: connecting the socket\n", CMD);
  ret = connect(sock, (struct sockaddr *)adrServ, sizeof(struct sockaddr_in));
  if (ret < 0)
    erreur_IO("connect");


  while (!fin) {
    printf("ligne> ");
    if (fgets(ligne, LIGNE_MAX, stdin) == NULL) {
      printf("arret par CTRL-D\n");
      fin = VRAI;
    }
    else {
      lgEcr = ecrireLigne(sock, ligne);
      if (lgEcr == -1)
        erreur_IO("ecrireLigne");
    
      printf("%s: %d octets envoyes\n", CMD, lgEcr);
    }
  }

  if (close(sock) == -1)
    erreur_IO("close");

  exit(EXIT_SUCCESS);
}
