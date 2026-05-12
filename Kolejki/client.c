#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "common.h"

int init_connection(key_t key, int* my_id) {
    int msqid = msgget(SERVER_KEY, IPC_CREAT | 0660);
    if (msqid == -1) { perror("Cannot create queue"); }
    struct msgbuf msg;
    msg.mtype = INIT_MTYPE;
    
    char key_str[32];
    snprintf(key_str, sizeof(key_str), "%d", (int)key);
    strcpy(msg.mtext, key_str);

    msgsnd(msqid, &msg, MSG_SIZE, 0);
    
    // odbieramy od serwera teraz
    int client_id = msgget(key, 0);
    struct msgbuf rec_init;
    size_t ret_init = msgrcv(client_id, &rec_init, MSG_SIZE, INIT_MTYPE, 0);
    if (ret_init >= 0) {
        printf("Dostalismy identifikator od serwera: %s\n", rec_init.mtext);
    }

    *my_id = atoi(rec_init.mtext);
    return msqid;
}

int main() {
    // Create own queue
    fclose(fopen("/tmp/chat_client", "a"));
    key_t user_key = ftok("/tmp/chat_client", getpid());
    int user_id = msgget(user_key, IPC_CREAT | 0660);    

    // Server connection
    int my_id;
    int server_id = init_connection(user_key, &my_id);
    pid_t pid = fork();
    if (pid == 0) {
        // Odbieranie pozostalych wiadomosci od serwera
        while (1) {
            struct msgbuf rec_init;
            size_t ret_init = msgrcv(user_id, &rec_init, MSG_SIZE, MESSAGE_MTYPE, 0);
            if (ret_init >= 0) {
                printf("Od %d: %s\n", rec_init.client_id, rec_init.mtext);
            }
        }
    } else {
        // Sending message to server
        while(1) {
            char buf[256];
            fgets(buf, sizeof(buf), stdin);
            
            struct msgbuf msg;
            msg.mtype = MESSAGE_MTYPE;
            msg.client_id = my_id;
            strcpy(msg.mtext, buf);
            msgsnd(server_id, &msg, MSG_SIZE, 0);
        }
    }
	return 0;
}