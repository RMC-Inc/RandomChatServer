#include "connection.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>



Connection* createConnection(User* user){
    Connection* conn = malloc(sizeof(Connection));
    conn->user1 = user;
    conn->user2 = NULL;
    conn->status = -1;
    conn->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    pipe(conn->pipefd);
    fcntl(conn->pipefd[0], F_SETFL, O_NONBLOCK); // set non block reading;
    conn->timer = NULL;
    return conn;
}

void setConnectionTimeout(Connection* conn, unsigned int timeout){
    conn->timer = newTimer(timeout, conn->pipefd[1]);
}

void connectUser(Connection* conn, User* user){
    conn->user2 = user;
    conn->status++;
    char c = 'c';
    write(conn->pipefd[1], &c, 1);
}

void closeConnection(Connection* conn){
    pthread_mutex_lock(&conn->mutex);
    conn->status++;
    pthread_mutex_unlock(&conn->mutex);

    if(conn->status == 2) {
        close(conn->pipefd[0]);
        close(conn->pipefd[1]);
        deleteTimer(conn->timer);
        free(conn);
    }
}

int isOpen(const Connection* conn){
    return conn->status == 0 && (conn->timer == NULL || !conn->timer->isEnd);
}
