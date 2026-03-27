#ifndef SIGNALS_LIB_H
#define SIGNALS_LIB_H

#include <stdio.h>
#include <signal.h>

void sig_default();
void sig_ignore();
void sig_mask();
void handler(int signum);

#endif