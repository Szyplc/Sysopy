#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include "common.h"

void print_array(int* array, int len) {
    printf("Array: \n");
    for(int i=0; i<len; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

void handle_init_user(struct msgbuf rec_init, int* user_array, int* user_count) {
    int user_id = msgget(atoi(rec_init.mtext), IPC_CREAT | 0660);
    user_array[*user_count] = user_id;

    struct msgbuf msg;
    msg.mtype = INIT_MTYPE;
    snprintf(msg.mtext, sizeof(msg.mtext), "%d", *user_count);
    msgsnd(user_id, &msg, MSG_SIZE, 0);
    
    *user_count += 1;
}

void send_message_to_user(int user_id, char* message, int client_id) {
    struct msgbuf msg;
    msg.mtype = MESSAGE_MTYPE;
    strcpy(msg.mtext, message);
    msg.client_id = client_id;
    msgsnd(user_id, &msg, MSG_SIZE, 0);
}

void send_message_to_all_users(int* array, int len, char* message, int client_id) {
    for (int i=0; i<len; i++) {
        if (i != client_id)
            send_message_to_user(array[i], message, client_id);
    }
}

int main() {
    // clean up
    int old_id = msgget(SERVER_KEY, 0);
    if (old_id != -1) {
        printf("Kolejka istnieje usuwam ja!\n");
        msgctl(old_id, IPC_RMID, NULL);
    }

    int server_id = msgget(SERVER_KEY, IPC_CREAT | 0660);
    int user_count = 0;
    int* user_array = malloc(sizeof(int) * USER_COUNT);

    while(1) {
        // Czekamy na INIT
        struct msgbuf rec_init;
        ssize_t ret_init = msgrcv(server_id, &rec_init, MSG_SIZE, INIT_MTYPE, IPC_NOWAIT);
        
        if (ret_init >= 0) {
            printf("Nowy uzytkownik!\n");
            printf("Wiadomosc: %s\n", rec_init.mtext);
            handle_init_user(rec_init, user_array, &user_count);
            print_array(user_array, user_count);
            printf("\n");
        }

        // Czekamy na wiadomosci
        struct msgbuf rec_message;
        ssize_t ret_message = msgrcv(server_id, &rec_message, MSG_SIZE, MESSAGE_MTYPE, IPC_NOWAIT);
        if (ret_message >= 0) {
            printf("Od %d, Wiadomosc: %s\n", rec_message.client_id, rec_message.mtext);
            // Propagowanie do pozostalych klientow
            send_message_to_all_users(user_array, user_count, rec_message.mtext, rec_message.client_id);
        }
        usleep(10000); // zabezpieczenie przed kreceniem sie w kolo i zuzywanie 100% CPU
    }
    return 0;
}