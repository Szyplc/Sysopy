#include "definitions.h"

volatile int mode = -1;

void sig_unblock() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

void receive_config(int sig, siginfo_t *info, void *ucontext) {
    mode = info->si_value.sival_int;
}

int main() {
    struct sigaction sa;
    sa.sa_sigaction = receive_config;
    sa.sa_flags = SA_SIGINFO; // dzieki tego mamy info o akcji
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR2, &sa, NULL);

    while (mode == -1) {
        pause(); 
    }

    if (mode == 0) {
        signal(SIGUSR2, SIG_DFL);
        sig_default();
    } else if (mode == 1) {
        sig_default(); // musimy ustawic domyslna bo sigaction teraz przejmuje obsluge sygnalu
        sig_mask();
    } else if (mode == 2) {
        sig_ignore();
    } else if (mode == 3) {
        sig_handle();
    }

    for (int i = 1; i <= 20; i++) {
        printf("i = %d\n", i);
        
        if (i == 5 || i == 15) {
            printf("Wysyłam sygnał USR2\n");
            raise(SIGUSR2);
        }
        
        if (i == 10) {
            sigset_t pending;
            sigpending(&pending);
            if (sigismember(&pending, SIGUSR2)) {
                printf("Odblokowuję USR2\n");
                sig_unblock();
            }
        }
        sleep(1);
    }

    printf("Pętla została wykonana w całości.\n");
    return 0;
}