#include <stdio.h>

int main(int argc, char const *argv[])
{
    printf("Je suis le PREMIER fils de mon père.\n");
    printf("Voici mon PID : %d\n", getpid());
    printf("et celui de mon père : %d\n", getppid());
    if(argc > 2) {
        for(int i = 0 ; i < argc ; ++i) {
            printf("Argument %d : %s\n", i, argv[i]);
        }
    }
    sleep(10);
    return 10;
}
