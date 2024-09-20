#include <stdio.h>

int main(int argc, char const *argv[])
{
    printf("Je suis le DEUXIEME fils de mon père.\n");
    printf("Voici mon PID : %d\n", getpid());
    printf("et celui de mon père : %d\n", getppid());
    sleep(10);
    return 11;
}
