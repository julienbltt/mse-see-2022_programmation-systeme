// serveur.c
#include "pse.h"

#define    CMD      "SERVEUR"

#define NOM_FIFO      "fifo"
#define NOM_JOURNAL   "journal.log"
#define NBR_WORKER 20

void remiseAZeroJournal(void);

// Global
pthread_mutex_t mutex;
int fdJournal;
DataSpec Workers_DataSpecStructure[NBR_WORKER];

void *thread_client(void *arg)
{
  DataSpec* data = (DataSpec*)arg;
  char ligne[LIGNE_MAX];            // Message reçu du client.
  char journalLigne[LIGNE_MAX+512];
  int lgLue, lgEcr;
  int session_isAlive = VRAI;
  while(VRAI) {

    sem_wait(&data->sem); //while(data->canal == -1) {usleep(100);}

    session_isAlive = VRAI;

    printf("[CLIENT] > Start dialogue\n");
    printf("[%s] > WORKER %d wake up.\n", CMD, (int)data->id);

    while(session_isAlive) {
      lgLue = lireLigne(data->canal, ligne); // Lecture des données client.
      if (lgLue < 0) // Erreur de lecture des données
        erreur_IO("lireLigne");
      else if (lgLue == 0 || strcmp(ligne, "fin") == 0 ) { // Arret du client distant
        printf("[%s] > Arret du client\n", CMD);
        session_isAlive = FAUX;
      }
      else if (strcmp(ligne, "reset") == 0) {
        printf("[%s] > Reset du journal.\n", CMD);
        remiseAZeroJournal();
      }
      else {
        // Formatage des données du journal
        time_t Time_Current = time(NULL);
        struct tm TimeStruct_Current = *localtime(&Time_Current);
        sprintf(journalLigne, "[CLIENT:%s] (%d-%02d-%02d %02d:%02d:%02d) > %s", 
                stringIP(ntohl(data->addr.sin_addr.s_addr)),
                TimeStruct_Current.tm_year + 1900,
                TimeStruct_Current.tm_mon + 1,
                TimeStruct_Current.tm_mday,
                TimeStruct_Current.tm_hour,
                TimeStruct_Current.tm_min,
                TimeStruct_Current.tm_sec,
                ligne);
        // Ecriture des données reçu dans un journal.
        lgEcr = ecrireLigne(fdJournal, journalLigne);
        if (lgEcr == -1)
          erreur_IO("ecriture journal");

        // Affichage du message dans la console
        printf("[CLIENT:%d] > (%dB) %s",(int)data->id, lgLue, ligne);
      }
    }

    if (close(data->canal) == -1)
      erreur_IO("close canal");

    pthread_mutex_lock(&mutex);
    data->canal = -1;
    pthread_mutex_unlock(&mutex);

    printf("[%s] > WORKER %d is free.\n", CMD, (int)data->id);

    printf("[CLIENT:%d] > Fin de communication\n",(int)data->id);
  }
}

void Worker_Create(void)
{
  for(int i = 0 ; i < NBR_WORKER; ++i) {
    Workers_DataSpecStructure[i].canal = -1;
    sem_init(&Workers_DataSpecStructure[i].sem, 0, 0);
    pthread_create(&(Workers_DataSpecStructure[i].id), NULL, thread_client, &Workers_DataSpecStructure[i]);
  }
  printf("[%s] > Workers create.\n", CMD);
}

int main(int argc, char *argv[]) {
  short port;
  int ecoute, canal, ret;
  struct sockaddr_in adrEcoute, adrClient;
  unsigned int lgAdrClient;
  int workers_isBussy = FAUX;

  Worker_Create();

  // Traitement des arguments passés en paramètre.
  if (argc != 2)
    erreur("usage: %s port\n", argv[0]);

  port = (short)atoi(argv[1]); // Formatage du PORT

  // Création du socket serveur
  printf("[%s] > Creating a socket\n", CMD);
  ecoute = socket (AF_INET, SOCK_STREAM, 0);
  if (ecoute < 0)
    erreur_IO("socket");

  // Configuration du socket serveur
  adrEcoute.sin_family = AF_INET;
  adrEcoute.sin_addr.s_addr = INADDR_ANY;
  adrEcoute.sin_port = htons(port);
  printf("[%s] > Binding to INADDR_ANY address on port %d\n", CMD, port);
  ret = bind (ecoute,  (struct sockaddr *)&adrEcoute, sizeof(adrEcoute));
  if (ret < 0)
    erreur_IO("bind");

  // Lancement de l'ecoute du serveur.
  printf("[%s] > Listening to socket\n", CMD);
  ret = listen (ecoute, 5);
  if (ret < 0)
    erreur_IO("listen");

  // Ouverture du journal de log.
  fdJournal = open(NOM_JOURNAL, O_CREAT|O_WRONLY|O_APPEND, 0600);
  if (fdJournal == -1)
    erreur_IO("ouverture journal");

  // En cours d'acceptation des connexion.
  printf("[%s] > Accepting a connection\n", CMD);

  while (VRAI) {

    // Ouverture d'une connexion avec un client.
    lgAdrClient = sizeof(adrClient);
    canal = accept(ecoute, (struct sockaddr *)&adrClient, &lgAdrClient);
    if (canal < 0)
      erreur_IO("accept");

    // Affichage du client
    printf("[%s] > ADDR : %s, PORT : %hu\n", CMD,
        stringIP(ntohl(adrClient.sin_addr.s_addr)),
        ntohs(adrClient.sin_port)); // Afficher les informations du client connecté.
    
    // Recherche d'un thread libre et reveiller le thread.
    do {
      for(int i = 0 ; i < NBR_WORKER ; ++i) {
        if( Workers_DataSpecStructure[i].canal == -1) {
          workers_isBussy = FAUX;
          Workers_DataSpecStructure[i].addr = adrClient;
          sem_post(&Workers_DataSpecStructure[i].sem);
          Workers_DataSpecStructure[i].canal = canal;
          break;
        }
        else if( i == NBR_WORKER) {
          workers_isBussy = VRAI;
        }
      }
    } while(workers_isBussy);

  }

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
