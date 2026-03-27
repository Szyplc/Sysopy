#include <signal.h>

void sig_ignore() {
    signal(SIGUSR2, SIG_IGN);
}