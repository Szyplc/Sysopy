#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

void sig_default();
void sig_mask();
void sig_ignore();
void sig_handle();