#include <stdio.h>
#include "pse.h"

int main(int argc, char const *argv[])
{
    int pid1, pid2;
    char* son1_args[] = {"son1", "argument1", "argument2", NULL};
    char* son2_args[] = {"son2", NULL};
    int son1_exitvalue, son2_exitvalue;


    pid1 = fork();
    if( pid1 == 0) {
        // Instructions fils.
        printf("Le PID du PREMIER fils : %d\n", execv("son1", son1_args));
    }
    else {
        pid2 = fork();
        if( pid2 == 0) {
            // Instruction du fils
            printf("Le PID du DEUXIEME fils : %d\n", execv("son2", son2_args));
        }
        else {
            // Instruction du père
            printf("Le PID du père : %d\n", getpid());
            wait(&son1_exitvalue); // Attendre fin du premier fils.
            wait(&son2_exitvalue); // Attendre fin du deuxième fils.
            printf("La valeur de fermeture du premier fils : %d\n", WEXITSTATUS(son1_exitvalue));
            printf("La valeur de fermeture du deuxième fils : %d\n", WEXITSTATUS(son2_exitvalue));
        }
    }
    return 0;
}
