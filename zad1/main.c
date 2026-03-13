#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unistd.h>

#define M 2

int zmiennaGlobalna = 10;
int main(int argc, char *argv[]) {
    int N = atoi(argv[1]);
    pid_t id;
    for(int i=0;i<N;i++) {
        id = fork();
        if(id == 0) break;
    }

    zmiennaGlobalna++;
    if(id == 0) { // tylko id != 0 ma proces macierzysty
        for(int i=0;i<M;i++) {
            printf("Potomek: %d\n", getpid());
            usleep(250000);
        }
        exit(0); // nic to nie zmienia ale podobno poprawna praktyka
    } else {
        for(int i=0;i<N;i++) {
            wait(NULL);
        } // czekamy na N procesow ktore stworzylismy
        printf("Rodzic: (%d), zmiennaGlobalna=%d\n", getpid(), zmiennaGlobalna);
    }
    return 0;
}

/*
1. Procesy potomne wypisuja w sposob pomieszany
2. Zmienna zmiennaGlobalna jest osobna dla kazdego procesu.
3. Wywolanie funkcji vfork spowoduje ze kazde dodanie do zmiennej zmiennaGlobalna
spowoduje faktyczne dodanie jej poniewaz wtedy pamiec jest wspolna dla wszystkich procesow
oraz proces macierzysty bedzie czekal na procesy potomne az one skoncza.
4. Logi i procesy sa rownoległe i system operacyjny decyduje kiedy kazdy proces
dostanie czas do procesora.
*/