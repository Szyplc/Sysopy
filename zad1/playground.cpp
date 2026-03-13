#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int zmiennaGlobalna = 10;

int main() {
    printf("Rodzic przed vfork: PID=%d, zmienna=%d\n", 
            getpid(), zmiennaGlobalna);

    pid_t id = vfork();

    if(id == 0) {
        // potomek — współdzieli pamięć rodzica!
        printf("Potomek działa: PID=%d\n", getpid());
        zmiennaGlobalna = 99; // modyfikuje WSPÓLNĄ pamięć!
        printf("Potomek zmienił zmienną na: %d\n", zmiennaGlobalna);
        exit(0); // ← odblokowuje rodzica
    }

    // rodzic — dociera tutaj dopiero po exit() potomka
    printf("Rodzic po vfork:  PID=%d, zmienna=%d\n", 
            getpid(), zmiennaGlobalna);

    return 0;
}