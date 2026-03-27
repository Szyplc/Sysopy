#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Wywołanie: %s default|mask|ignore|handle\n", argv[0]);
        exit(1);
    }

    union sigval value;
    if (strcmp(argv[1], "default") == 0) value.sival_int = 0;
    else if (strcmp(argv[1], "mask") == 0) value.sival_int = 1;
    else if (strcmp(argv[1], "ignore") == 0) value.sival_int = 2;
    else if (strcmp(argv[1], "handle") == 0) value.sival_int = 3;
    else {
        printf("Nieznany argument: %s\n", argv[1]);
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        exit(1);
    }

    if (pid == 0) {
        execl("./child", "child", NULL);
        exit(1);
    } else {
        sleep(1); 
        
        if (sigqueue(pid, SIGUSR2, value) == -1) { // tutaj wysyłamy sygnał z konfiguracją
            exit(1);
        }

        wait(NULL); // czekamy na zakończenie dziecka
    }

    return 0;
}