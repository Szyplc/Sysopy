#include "definitions.h"

int main(int argc, char *argv[]) {
    int M = atoi(argv[1]);
    FILE *f = fopen(name, "a");

    int fd = fileno(f);  // deskryptor pliku

    flock(fd, LOCK_EX);  // dajemy blokade procesowi na wylacznosc czyli

    for(int i=0;i<M;i++) {
        fprintf(f, "Potomek: %d\n", getpid());
        fflush(f);  // od razu to co zapisal linijke wyzej od razu zapisuje do pliku
        usleep(250000);
    }
    fclose(f);
    exit(0);
}