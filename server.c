// -------------
// SERVEUR
// -------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#include "msg.h"

#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}
#define MSG_KEY 12597

int reader_fifo, id_file;
key_t tx_key;
pthread_t thread_read;


void initTube() {
    mkfifo("game.fifo",0666);
    reader_fifo = open("game.fifo", O_RDONLY);
}

void initMsg() {
   tx_key = ftok("/tmp", MSG_KEY);
   id_file = msgget(tx_key, 0666 | IPC_CREAT);
   CHECK(id_file, "Impossible de créer le flux de message");
}

void sendMessageToClient(char msg[256]) {
    int res;
    msg_t message;
    message.type = 1;
    strncpy(message.msg, msg, 256);
    res = msgsnd(id_file, (void *) &message, sizeof(message.msg), 0);
    CHECK(res, "msgsnd()");
}


void *reader() {
    initTube();
    char msg[256];
    do {
        read(reader_fifo, msg, sizeof(msg));
        printf("RX : %s\n",msg);
    } while (strcmp(msg, "STOP\n") != 0);
    close(reader_fifo);
}

void emptybuff() {
    int c = 0;
    while (c!='\n' && c!=EOF) {
        c = getchar();
    }
}

int readline(char *chaine, int length) {
    char *start = NULL;
    if (fgets(chaine, length, stdin) != NULL) {
        start = strchr(chaine, '\n');
        if (start != NULL) {
            *start = '\0';
        }else {
            emptybuff();
        }
        return 1;
    } else {
        emptybuff();
        return 0;
    }
}

int main(void) {
    pthread_create(&thread_read, NULL, reader, NULL);

    char buffer[256];
    initMsg();
    while (strcmp(buffer, "STOP")) {
        printf("> ");
        readline(buffer, 256);
        sendMessageToClient(buffer);
    }


    pthread_join(thread_read, NULL);
}