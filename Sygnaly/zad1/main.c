#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void sig_default() {
    signal(SIGUSR1, SIG_DFL);
}

void sig_ignore() {
    signal(SIGUSR1, SIG_IGN);
}

void sig_mask() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);

    sigprocmask(SIG_BLOCK, &mask, NULL);
}

void sig_unblock() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

void handler(int signum) {
    printf("Wywołano handler dla sygnału %d!\n", signum);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Wywołanie: %s default|mask|ignore|handle\n", argv[0]);
        exit(1);
    }
    char *r = argv[1];
    if (strcmp(r, "default") == 0) {
        sig_default();
    } else if (strcmp(r, "mask") == 0) {
        sig_mask();
    } else if (strcmp(r, "ignore") == 0) {
        sig_ignore();
    } else if (strcmp(r, "handle") == 0) {
        signal(SIGUSR1, handler);
    } else {
        printf("Nieznany argument: %s\n", r);
        exit(1);
    }
    printf("Wywolujemy funkcje: %s\n", r);

    for(int i=1; i<=20; i++) {
        printf("i = %d\n", i);
        if (i == 5 || i == 15) {
            printf("Wysylam sygnal USR1!\n");
            raise(SIGUSR1);
        }
        if (i == 10) {
            sigset_t pending;
            sigpending(&pending);
            if (sigismember(&pending, SIGUSR1)) {
                sig_unblock();
            }
        }
        sleep(1);
    }
    printf("Pętla została wykonana w całości. \n");
    return 0;
}