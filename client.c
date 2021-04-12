// -------------------
// CLIENT
// -------------------

// Se connecte au tube / bal que le serveur à créé
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
int writer_fifo, id_msg;
key_t rx_key;
pthread_t thread_read;

void initTube() {
    mkfifo("game.fifo", 0666);
    writer_fifo = open("game.fifo", O_WRONLY);
}

void initMsg() {
    rx_key = ftok("/tmp",MSG_KEY);
    CHECK(id_msg = msgget(rx_key, 0666),"msgget()");
}

void *reader() {
    initMsg();
    msg_t message;
    while (strcmp(message.msg, "STOP") != 0) {
        CHECK(msgrcv(id_msg,&message, sizeof(message.msg), 0,MSG_NOERROR),"msgrcv()");
        printf("RX: %s\n",message.msg);
    }
    printf("Fin de lecture\n");
}

void sendMsg(char message[256]) {
    printf("Envoi de : %s\n", message);
    write(writer_fifo, message, strlen(message)+1);
}


int main(void) {
    pthread_create(&thread_read, NULL, reader, NULL);
    initTube();
    char message[256];
    do {
        fgets(message, 256, stdin);
        sendMsg(message);
    } while (strcmp(message, "STOP") != 0);

    pthread_join(thread_read, NULL);
    return 0;
}