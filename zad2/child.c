#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int M = atoi(argv[1]);

    for(int i=0;i<M;i++) {
        printf("Potomek: %d\n", getpid());
        usleep(250000);
    }

    exit(0);
}