#include "pse.h"
#include "tampon.h"

#define N 1
#define M 2

typedef struct {
    int thread_id;
    int tempo_ms;    
} Thread_ArgTypeDef;

sem_t ThreadProducteur_Sem;
sem_t ThreadConsommateur_Sem;

Tampon Tampon_Structure;

Thread_ArgTypeDef Thread_ArgListStructure[M];
pthread_mutex_t myMultex = PTHREAD_MUTEX_INITIALIZER;;

void *Thread_Producteur(void *arg)
{
     int *tempo_ms = (int *)arg;
     int Thread_ProducteurIndex = 0;

     printf("Début du Thread Producteur.\n");
     while(VRAI) {
        // Attente de place libre dans le tampo par sémpahore
        sem_wait(&ThreadProducteur_Sem);

        // Temporisation 1s
        usleep(*tempo_ms*100);

        // Ajoute une valeur dans le tampon.
        pthread_mutex_lock(&myMultex);
        tampon_deposer(&Tampon_Structure, Thread_ProducteurIndex++);
        sem_post(&ThreadConsommateur_Sem);
        pthread_mutex_unlock(&myMultex);
     }

     pthread_exit(NULL);
}

void *Thread_Consommateur(void *arg)
{
    Thread_ArgTypeDef *Thread_ArgStructure = (Thread_ArgTypeDef *)arg;
    int Thread_ConsommateurValue = 0;
    printf("Début du Thread Consommateur.\n");
    while(VRAI) {
        // Attente de place libre dans le tampo par sémpahore
        sem_wait(&ThreadConsommateur_Sem);

        // Temporisation 0.1s
        usleep(Thread_ArgStructure->tempo_ms*100);

        // Prend une valeur dans le tampon.
        pthread_mutex_lock(&myMultex);
        tampon_prendre(&Tampon_Structure, &Thread_ConsommateurValue);
        sem_post(&ThreadProducteur_Sem);
        pthread_mutex_unlock(&myMultex);

        // Affichage de la valeur lu.
        printf("[Thread:%d] > Value : %d\n", Thread_ArgStructure->thread_id, Thread_ConsommateurValue);
    }

    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    int ret;
    int ThreadProducteur_Tempo = atoi(argv[1]), ThreadConsommateur_Tempo = atoi(argv[2]);
    pthread_t Thread_ProductionID, Thread_ConsommateurID;
    tampon_init(&Tampon_Structure);
    sem_init(&ThreadProducteur_Sem, 0, P);
    sem_init(&ThreadConsommateur_Sem, 0, 0);
    
    // Création des Producteurs
    for(int i = 0 ; i < N ; ++i ) {
        ret = pthread_create(&Thread_ProductionID, NULL, Thread_Producteur, &ThreadProducteur_Tempo);
        if (ret != 0)
            erreur_IO("pthread_create");
    }
    
    // Création des Consomateur
    for(int i = 0 ; i < M ; ++i) {
        Thread_ArgListStructure[i].thread_id = i+1;
        Thread_ArgListStructure[i].tempo_ms = ThreadConsommateur_Tempo;
        ret = pthread_create(&Thread_ConsommateurID, NULL, Thread_Consommateur, &Thread_ArgListStructure[i]);
        if (ret != 0)
            erreur_IO("pthread_create");
    }

    // while(VRAI) {
    //     printf("[");
    //     for(int i = 0 ; i < P-1 ; ++i) {
    //         printf("%d,", Tampon_Structure.buffer[i]);
    //     }
    //     printf("%d]\n", Tampon_Structure.buffer[P-1]);
    //     sleep(1);
    // }

    pthread_join(Thread_ProductionID, NULL);
    pthread_join(Thread_ConsommateurID, NULL);
    return 0;
}
