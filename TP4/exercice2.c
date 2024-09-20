#include "pse.h"

#define SIZE_NAME 1024

int main(int argc, char const *argv[])
{
    char file_name[SIZE_NAME], buffer[SIZE_NAME];
    char* nano_args[] = {"cat", NULL, NULL};
    int pid;
    int exitvalue;

    printf("Entrer un nom de fichier : ");
    fgets(file_name, SIZE_NAME, stdin);

    file_name[strcspn(file_name, "\n")] = 0;
    nano_args[1] = file_name;

    pid = fork();
    if(pid == 0) {
        // Instrucitons fils.
        execv("/usr/bin/cat", nano_args);
    }
    else {
        // Instructions père.
        if(waitpid(pid, &exitvalue, WNOHANG) == 0) {
            printf("Voulez vous attendre ? [Y/n]");
            fgets(buffer, SIZE_NAME, stdin);
            if( !(strcmp(buffer,"\n")) || !(strcmp(buffer,"Y\n")) ) {
                printf("En attente de la fin du programme.\n");
                wait(&exitvalue);
            }
            printf("Fin du programme\n");
        }
    }
    
    return 0;
}
