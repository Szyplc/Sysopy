#include "definitions.h"

int main(int argc, char *argv[]) {
    int N = atoi(argv[1]);
    remove(name);
    
    for(int i=0;i<N;i++) {
        id = fork();
        if(id == 0) break;
    }

    if(id == 0) {
        execl("./child", "child", argv[2], NULL);
    } else {
        for(int i=0;i<N;i++) {
            wait(NULL);
        }
        // printf("Rodzic: (%d)\n", getpid());
    }

    exit(0);
}