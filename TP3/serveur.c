// serveur.c
#include "pse.h"

#define    CMD      "serveur"

#define NOM_FIFO      "fifo"
#define NOM_JOURNAL   "journal.log"

void remiseAZeroJournal(void);

int fdJournal;

int main(int argc, char *argv[]) {
  short port;
  int ecoute, canal, ret;
  struct sockaddr_in adrEcoute, adrClient;
  unsigned int lgAdrClient;
  char ligne[LIGNE_MAX];
  int lgLue, lgEcr;
  int fin = FAUX;
  int occupe = FAUX;

  // Traitement des arguments passés en paramètre.
  if (argc != 2)
    erreur("usage: %s port\n", argv[0]);

  port = (short)atoi(argv[1]); // Formatage du PORT

  // Création du socket serveur
  printf("%s: creating a socket\n", CMD);
  ecoute = socket (AF_INET, SOCK_STREAM, 0);
  if (ecoute < 0)
    erreur_IO("socket");

  // Configuration du socket serveur
  adrEcoute.sin_family = AF_INET;
  adrEcoute.sin_addr.s_addr = INADDR_ANY;
  adrEcoute.sin_port = htons(port);
  printf("%s: binding to INADDR_ANY address on port %d\n", CMD, port);
  ret = bind (ecoute,  (struct sockaddr *)&adrEcoute, sizeof(adrEcoute));
  if (ret < 0)
    erreur_IO("bind");

  // Lancement de l'ecoute du serveur.
  printf("%s: listening to socket\n", CMD);
  ret = listen (ecoute, 5);
  if (ret < 0)
    erreur_IO("listen");

  // Ouverture du journal de log.
  fdJournal = open(NOM_JOURNAL, O_CREAT|O_WRONLY|O_APPEND, 0600);
  if (fdJournal == -1)
    erreur_IO("ouverture journal");

  // En cours d'acceptation des connexion.
  printf("%s: accepting a connection\n", CMD);

  while (!fin) {
    if (!occupe) {
      // Ouverture d'une connexion avec un client.
      lgAdrClient = sizeof(adrClient);
      canal = accept(ecoute, (struct sockaddr *)&adrClient, &lgAdrClient);
      if (canal < 0)
        erreur_IO("accept");
      printf("%s: adr %s, port %hu\n", CMD,
          stringIP(ntohl(adrClient.sin_addr.s_addr)),
          ntohs(adrClient.sin_port)); // Afficher les informations du client connecté.
      occupe = VRAI;
    }

    lgLue = lireLigne(canal, ligne); // Lecture des données client.
    if (lgLue < 0) // Erreur de lecture des données
      erreur_IO("lireLigne");
    else if (lgLue == 0 || strcmp(ligne, "fin") == 0 ) { // Arret du client distant
      printf("%s: arret du client\n", CMD);
      occupe = FAUX;
    }
    else if ( strcmp(ligne, "shutdown") == 0 ){
      printf("[%s]. Demande de fin de communication.\n", CMD);
      fin = VRAI;
    }
    else if (strcmp(ligne, "reset") == 0) {
      printf("[%s]. Reset du journal.\n", CMD);
      remiseAZeroJournal();
    }
    else {
      // Ecriture des données reçu dans un journal.
      lgEcr = ecrireLigne(fdJournal, ligne); 
      if (lgEcr == -1)
        erreur_IO("ecriture journal");

      // Affichage du message dans la console
      printf("[%s] > [%do] Message : %s", CMD, lgLue, ligne);
    }
  } 

  if (close(canal) == -1)
    erreur_IO("close canal");

  if (close(ecoute) == -1)
    erreur_IO("close ecoute");

  if (close(fdJournal) == -1)
    erreur_IO("fermeture journal");  

  exit(EXIT_SUCCESS);
}

void remiseAZeroJournal(void) {
  // on ferme le fichier et on le rouvre en mode O_TRUNC
  if (close(fdJournal) == -1)
    erreur_IO ("fermeture journal pour remise a zero");

  fdJournal = open(NOM_JOURNAL, O_TRUNC|O_WRONLY|O_APPEND, 0600);
  if (fdJournal == -1)
    erreur_IO ("ouverture journal pour remise a zero");
}
