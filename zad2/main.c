#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int N = atoi(argv[1]);
    
    pid_t id;
    for(int i=0;i<N;i++) {
        id = fork();
        if(id == 0) break;
    }

    if(id == 0) {
        execl("./child", "child", "2", NULL);
    } else {
        for(int i=0;i<N;i++) {
            wait(NULL);
        }
        printf("Rodzic: (%d)\n", getpid());
    }
    return 0;
}