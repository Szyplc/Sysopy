#include <signal.h>

void sig_default() {
    signal(SIGUSR2, SIG_DFL);
}