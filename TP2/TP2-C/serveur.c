// serveur.c
#include "pse.h"

#define NOM_FIFO      "fifo"
#define NOM_JOURNAL   "journal.log"

void remiseAZeroJournal(void);

int fdJournal;

int main(void) {
  int fdFifo;
  int fin = FAUX;
  char ligne[LIGNE_MAX];
  int lgLue, lgEcr;

  fdJournal = open(NOM_JOURNAL, O_CREAT|O_WRONLY|O_APPEND, 0600);
  if (fdJournal == -1)
    erreur_IO("ouverture journal");

  fdFifo = open(NOM_FIFO, O_RDONLY);
  if (fdFifo == -1)
    erreur_IO("ouverture fifo");

  while (!fin) {
    lgLue = lireLigne(fdFifo, ligne);
    if (lgLue == -1)
      erreur_IO("lecture ligne");

    if (lgLue == 0) {
      printf("Serveur. arret du client\n");
      fin = VRAI;
    }
    else if (strcmp(ligne, "fin") == 0) {
      printf("Serveur. fin demandee\n");
      fin = VRAI;
    }
    else if (strcmp(ligne, "init") == 0) {
      printf("Serveur. remise a zero du journal\n");
      remiseAZeroJournal();
    }
    else {
      lgEcr = ecrireLigne(fdJournal, ligne); 
      if (lgEcr == -1)
        erreur_IO("ecriture journal");

      printf("Serveur. ligne de %d octets ecrite dans le journal\n", lgEcr);
    }
  }

  if (close(fdFifo) == -1)
    erreur_IO("fermeture fifo");  

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
