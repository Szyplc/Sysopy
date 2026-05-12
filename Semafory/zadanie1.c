#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <errno.h>

#define DATA_LEN 10

char* gen_ran_arr(int n) {
    char* res = malloc(n);
    for (int i=0; i<n; i++) {
        res[i] = 'a' + rand() % 26;
    }
    return res;
}

struct shared_buffer {
    int in;
    int out;
    int capacity;

    sem_t empty;
    sem_t full;
    sem_t mutex;
    char (*data)[DATA_LEN];
};

void save_data(struct shared_buffer* shared_memory, char* data) {
    sem_wait(&shared_memory->empty);
    sem_wait(&shared_memory->mutex);
    
    memcpy(shared_memory->data[shared_memory->in], data, DATA_LEN);
    shared_memory->in = (shared_memory->in + 1) % shared_memory->capacity;

    sem_post(&shared_memory->mutex);
    sem_post(&shared_memory->full);
}

char* receive_data(struct shared_buffer* shared_memory) {
    sem_wait(&shared_memory->full);
    sem_wait(&shared_memory->mutex);

    char* data = malloc(DATA_LEN);
    memcpy(data, shared_memory->data[shared_memory->out], DATA_LEN);
    shared_memory->out = (shared_memory->out + 1) % shared_memory->capacity;

    sem_post(&shared_memory->mutex);
    sem_post(&shared_memory->empty);

    return data;
}

void init_semaphore(struct shared_buffer* B, int N) {
    B->in = 0;
    B->out = 0;
    B->capacity = N;
    sem_init(&B->mutex, 1, 1);
    sem_init(&B->full, 1, 0);
    sem_init(&B->empty, 1, N);
    B->data = (char (*)[DATA_LEN])((char *)B + sizeof(struct shared_buffer));
}

// docker run -it --rm -v "$(pwd):/work" gcc bash
int main(int argc, char *argv[]) {
    // Getting arguments
    if (argc < 4) {
        printf("Arguments not found!\n");
        exit(EXIT_FAILURE);
    }
    int N = atoi(argv[1]);
    int M = atoi(argv[2]);
    int K = atoi(argv[3]);
    
    // Shared memory setup
    size_t shm_size = sizeof(struct shared_buffer) + K * DATA_LEN;
    int shmid = shmget(IPC_PRIVATE, shm_size, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    
    struct shared_buffer *shared_memory = shmat(shmid, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat");
        shmctl(shmid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }
    init_semaphore(shared_memory, K);

    // Producent creating
    pid_t* producent_arr = malloc(sizeof(pid_t) * N);
    for (int i=0; i<N; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        
        if (pid == 0) {
            // Producent
            srand(time(NULL) ^ getpid());
            char* random_str = gen_ran_arr(DATA_LEN);
            save_data(shared_memory, random_str);
            free(random_str);
            exit(0);
        } else {
            producent_arr[i] = pid;
        }
    }

    // Konsument creating
    for (int i=0; i<M; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Konsument
            while(1) {
                char* data = receive_data(shared_memory);
                if (data[0] == '\0') {
                    free(data);
                    break;
                }

                for (int i=0; i<DATA_LEN; i++) {
                    printf("%c", data[i]);
                    fflush(stdout);
                    usleep(300000);
                }
                printf("\n");
                free(data);
            }
            exit(0);
        }
    }
    
    // Czekamy na producentow
    for (int i=0; i<N; i++) {
        waitpid(producent_arr[i], NULL, 0);
    }

    // Poison pill
    for (int i=0; i<M; i++) {
        char poison[DATA_LEN] = {0};
        save_data(shared_memory, poison);
    }

    // Czekamy na konsumentow
    for (int i=0; i<M; i++) {
        wait(NULL);
    }

    // Czyszczenie
    sem_destroy(&shared_memory->mutex);
    sem_destroy(&shared_memory->empty);
    sem_destroy(&shared_memory->full);
    shmdt(shared_memory);
    shmctl(shmid, IPC_RMID, NULL);

    return  0;
}