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
    int in_normal;
    int out_normal;
    
    int in_priority;
    int out_priority;

    int capacity;

    sem_t empty_normal;
    sem_t full_normal;

    sem_t empty_priority;
    sem_t full_priority;

    sem_t full_any;
    sem_t mutex;

    char (*normal_queue)[DATA_LEN];
    char (*priority_queue)[DATA_LEN];
};

void push_queue(sem_t *empty, sem_t *full, sem_t *mutex, sem_t *full_any,
                char (*queue)[DATA_LEN], int *in, int capacity, const char *data
            ) {
                sem_wait(empty);
                sem_wait(mutex);

                memcpy(queue[*in], data, DATA_LEN);
                *in = (*in + 1) % capacity;

                sem_post(mutex);
                sem_post(full);
                sem_post(full_any);
            }

void save_data(struct shared_buffer* B, char* data) {
    // random where data goes to which queue
    int r = rand() % 10;
    if (r < 3) { // priority queue
        push_queue(&B->empty_priority, &B->full_priority, &B->mutex, &B->full_any,
            B->priority_queue, &B->in_priority, B->capacity, data);
    } else { // normal queue
        push_queue(&B->empty_normal, &B->full_normal, &B->mutex, &B->full_any,
            B->normal_queue, &B->in_normal, B->capacity, data);
    }    
}

char* receive_from_queue(sem_t *empty, sem_t *mutex,
                char (*queue)[DATA_LEN], int *out, int capacity
            ) {
                sem_wait(mutex);

                char* data = malloc(DATA_LEN);
                memcpy(data, queue[*out], DATA_LEN);
                *out = (*out + 1) % capacity;

                sem_post(mutex);
                sem_post(empty);

                return data;
            }

char* receive_data(struct shared_buffer* B) {
    sem_wait(&B->full_any);
    if (sem_trywait(&B->full_priority) == 0) { // wersja nieblokujaca
        return receive_from_queue(&B->empty_priority, 
            &B->mutex, B->priority_queue, &B->out_priority, B->capacity);
    }

    if (errno != EAGAIN) {
        sem_post(&B->full_any);
        return NULL;
    };
    
    // nie ma zasobu wiecj jest w normal
    sem_wait(&B->full_normal);
    return receive_from_queue(&B->empty_normal, &B->mutex, 
        B->normal_queue, &B->out_normal, B->capacity);
}

void init_semaphore(struct shared_buffer* B, int K) {
    B->in_normal = 0;
    B->out_normal = 0;
    B->in_priority = 0;
    B->out_priority = 0;
    B->capacity = K;
    sem_init(&B->mutex, 1, 1);
    sem_init(&B->full_any, 1, 0);
    sem_init(&B->empty_normal, 1, K);
    sem_init(&B->full_normal, 1, 0);
    sem_init(&B->empty_priority, 1, K);
    sem_init(&B->full_priority, 1, 0);
    B->normal_queue = (char (*)[DATA_LEN])((char *)B + sizeof(struct shared_buffer));
    B->priority_queue = (char (*)[DATA_LEN])((char *)B + sizeof(struct shared_buffer) + K * DATA_LEN);
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
                if (data[0] == '\0' || data == NULL) {
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
        push_queue(&shared_memory->empty_normal, &shared_memory->full_normal,
           &shared_memory->mutex, &shared_memory->full_any,
           shared_memory->normal_queue, &shared_memory->in_normal,
           shared_memory->capacity, poison);
    }

    // Czekamy na konsumentow
    for (int i=0; i<M; i++) {
        wait(NULL);
    }

    // Czyszczenie
    sem_destroy(&shared_memory->mutex);
    sem_destroy(&shared_memory->empty_normal);
    sem_destroy(&shared_memory->full_normal);
    sem_destroy(&shared_memory->empty_priority);
    sem_destroy(&shared_memory->full_priority);
    shmdt(shared_memory);
    shmctl(shmid, IPC_RMID, NULL);

    return  0;
}