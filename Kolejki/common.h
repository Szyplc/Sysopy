#define SERVER_KEY 0x1234ABCD
#define INIT_MTYPE 1
#define MESSAGE_MTYPE 5
#define USER_COUNT 10

struct msgbuf {
    long mtype;
    int client_id;
    char mtext[256];
};

#define MSG_SIZE (sizeof(struct msgbuf) - sizeof(long))