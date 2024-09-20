#include "pse.h"

#define NBR_THREADS 10
int thread_ids[NBR_THREADS];
int calculs[NBR_THREADS];

void *maFonction(void *arg) {
    int *id = (int*)arg;
    printf("THREAD ID : %d\n", *id);
    for(int i = 0 ; i <= *id ; ++i) {
        calculs[*id] += i;
    }
    pthread_exit(NULL);
}

int main(void) {
    pthread_t idThread;

    for(int i = 0 ; i < NBR_THREADS ; ++i) {
        thread_ids[i] = i+1;
        int ret = pthread_create(&idThread, NULL, maFonction, &thread_ids[i]);
        if (ret != 0)
            erreur_IO("pthread_create");
    }
    
    for(int i = 0 ; i < NBR_THREADS ; ++i) {
        pthread_join(idThread, NULL);
    }

    for(int i = 0 ; i < NBR_THREADS ; ++i) {
        printf("La valeur du Thread %d : %d\n", thread_ids[i], calculs[i]);
    }

    exit(EXIT_SUCCESS);
}