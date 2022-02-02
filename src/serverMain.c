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
    int sortById(Room*, Room*);
    sortBy(roomVector, sortById);

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
    unsigned long long connectionCount = 1;
    while (1){
        printf("Await for client...\n");

        client = accept(server, NULL, NULL);
        if (client == -1){
            perror("Error client accept.\n"); perror(errno);
            exit(EXIT_FAILURE);
        }

        printf("New connection (n %llu) starting thread...\n", connectionCount);
        pthread_t tid;

        User* user = malloc(sizeof(User));
        user->socketfd = client;
        user->prev = 0;
        user->connectionCount = connectionCount++;
        if(connectionCount == 0) connectionCount++;

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

    User* user = (User*) arg;

    printf("[%llu] Thread started\n", user->connectionCount);

    // Setting nickname

    msglen = recv(user->socketfd, buff, BUFF_LEN, 0);
    if(msglen <= 0){
        printf("[%llu] Connection closed by client (nickname)\n", user->connectionCount);
        printf("[%llu] Closing connection\n", user->connectionCount);
        if(close(user->socketfd) < 0){
            perror("Error in Closing connection.\n");
        }
        free(user);
        pthread_exit(NULL);
    }
    buff[msglen] = '\0';

    if(!changeNickname(user, buff)){
        printf("[%llu] Invalid nickname, closing connection.\n", user->connectionCount);
        if(close(user->socketfd) < 0){
            printf("[%llu] Error in Closing connection.\n", user->connectionCount);
        }
        free(user);
        pthread_exit(NULL);
    }


    do{
        msglen = recv(user->socketfd, buff, BUFF_LEN, MSG_NOSIGNAL);
        if(msglen <= 0){
            if(errno == EINTR) continue;
            printf("[%llu] Connection closed by client\n", user->connectionCount);
            break;
        }
        buff[msglen] = '\0';
        printf("[%llu] Data recived from client: %s\n", user->connectionCount, buff);
    } while(dispatch(user, roomVector, buff[0], buff+1));

    printf("[%llu] Closing connection\n", user->connectionCount);
    if(close(user->socketfd) < 0){
        printf("[%llu] Error in Closing connection.\n", user->connectionCount);
    }
    free(user);
    pthread_exit(NULL);
}

int sortById(Room* a, Room* b){
    return a->id < b->id;
}
