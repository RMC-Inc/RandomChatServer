#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"
#include "FileManagement/fileManager.h"


void* clientHandler(void*);

RoomVector*  roomVector;
int main() {
// ----- Load rooms from file -----
    roomVector = newVector();

    // Todo load rooms
    //Room* r = newRoom("Stanza di prova1", 0, (unsigned char[]) {1 , 2, 4}, (unsigned char[]){1, 2, 4}, 0);
    //add(roomVector, r);
    //r = newRoom("Stanza di prova2", 5000, (unsigned char[]) {255 , 200, 55}, (unsigned char[]){22, 0, 100}, 5);
    //add(roomVector, r);

    // a r.g.b icon r.g.b t [roomname]
    // a 255.0.0 500 0.100.100 0 [Stanza di prova1]
    // a 0.255.0 500 100.0.100 0 [Stanza di prova2]
    // a 0.0.255 500 100.100.0 100 [Stanza di prova3]

    loadFromFile(roomVector, "rooms.rc");

    startAutoSave(roomVector, "rooms.rc", 60 * 5);

// ----- Starting server -----
    int server, client;

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    printf("Creating server socket\n");
    server = socket(PF_INET, SOCK_STREAM, 0);
    if(server < 0) {
        perror("Error Creating server socket.\n"); perror(errno);
        exit(EXIT_FAILURE);
    }


    printf("Binding server socket\n");
    if(bind(server, (struct sockaddr*) &address, sizeof(address)) < 0){
        perror("Error Binding.\n"); perror(errno);
        exit(EXIT_FAILURE);
    }

    printf("Set server to listening mode.\n");
    if(listen(server, 10) < 0){
        perror("Error Listen.\n"); perror(errno);
        exit(EXIT_FAILURE);
    }


// ----- Wait for client -----

    while (1){
        printf("Await for client...\n");

        client = accept(server, NULL, NULL);
        if (client == -1){
            perror("Error client accept.\n"); perror(errno);
            exit(EXIT_FAILURE);
        }

        printf("New connection starting thread...\n");
        pthread_t tid;

        User* user = malloc(sizeof(User));
        user->socketfd = client;
        user->prev_tid = -1;

        pthread_create(&tid, NULL, clientHandler, (void*) user);
        pthread_detach(tid);
        printf("Thread started.\n\n");
    }

    close(server);
    deleteVector(roomVector);
    return 0;
}



void* clientHandler(void* arg){
    char buff[BUFF_LEN];
    ssize_t msglen;
    pthread_t tid = pthread_self();

    printf("[t%ld] Thread started\n", tid);

    User* user = (User*) arg;
    user->tid = pthread_self();

    // Setting nickname

    msglen = recv(user->socketfd, buff, NICK_LEN, 0);
    if(msglen <= 0){
        fprintf(stderr, "[t%ld] Error recv nickname Closing connection\n", tid); perror(errno);
        close(user->socketfd);
        free(user);
        pthread_exit(NULL);
    }
    buff[msglen] = '\0';

    if(!changeNickname(user, buff)){
        fprintf(stderr, "[t%ld] Invalid nickname, closing connection.\n", tid);
        if(close(user->socketfd) < 0){
            perror("Error Closing connection.\n"); perror(errno);
        }
        free(user);
        pthread_exit(NULL);
    }


    do{
        msglen = recv(user->socketfd, buff, BUFF_LEN, MSG_NOSIGNAL);
        if(msglen <= 0){
            if(errno == EINTR) continue;
            fprintf(stderr, "[t%ld] Error connection closed by client\n", tid); perror(errno);
            break;
        }
        buff[msglen] = '\0';
        printf("[t%ld] Data recived from client: %s\n", tid, buff);
    } while(dispatch(user, roomVector, buff[0], buff+1));

    printf("[t%ld] Closing connection\n", tid);
    if(close(user->socketfd) < 0){
        perror("Error Closing connection.\n"); perror(errno);
    }
    free(user);
    pthread_exit(NULL);
}
