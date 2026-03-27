#include <signal.h>
#include <stdio.h>


void handler_usr2(int signum) {
    printf("Wywołano handler dla sygnału %d!\n", signum);
}

void sig_handle() {
    signal(SIGUSR2, handler_usr2);
}