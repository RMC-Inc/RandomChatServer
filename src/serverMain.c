#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8081
#define BUFF_LEN 500


void* clientHandler(void* client);

int main() {

    int server, client;

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    printf("Creating server socket\n");
    server = socket(PF_INET, SOCK_STREAM, 0);
    if(server < 0) {
        perror("Error Creating server socket.\n"); perror(errno);
        exit(1);
    }


    printf("Binding server socket\n");
    if(bind(server, (struct sockaddr*) &address, sizeof(address)) < 0){
        perror("Error Binding.\n"); perror(errno);
        exit(1);
    }

    printf("Set server to listening mode.\n");
    if(listen(server, 10) < 0){

    }

    while (1){
        printf("Await for client...\n");

        client = accept(server, NULL, NULL);
        if (client == -1){
            perror("Error client accept.\n"); perror(errno);
            exit(1);
        }

        printf("New connection starting thread...\n");
        pthread_t tid;
        int* th_clientsd = malloc(sizeof (int));
        *th_clientsd = client;
        pthread_create(&tid, NULL, clientHandler, (void*) th_clientsd);
        pthread_detach(tid);
        printf("Thread started.\n\n");
    }

    close(server);
    return 0;
}

void* clientHandler(void* clientsd){
    char buff[BUFF_LEN];
    ssize_t msglen = 0;
    int client = *((int*) clientsd);
    pthread_t tid = pthread_self();

    printf("[t%ld] Thread started\n", tid);
    strcpy(buff, "Hy client\n\0");
    write(client, buff, strlen(buff));
    while(1){
        msglen = read(client, buff, BUFF_LEN);
        buff[msglen] = '\0';
        printf("[t%ld] Data recived from client: %s\n", tid, buff);

        if(buff[0] == '-') break;

        printf("[t%ld] Send data to client\n", tid);
        write(client, buff, msglen);

    }
    printf("[t%ld] Closing connection\n", tid);
    close(client);
}
