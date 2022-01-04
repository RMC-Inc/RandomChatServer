#include "connection.h"
#include <stdlib.h>


Connection* createConnection(User* user){
    Connection* conn = malloc(sizeof(Connection));
    conn->user1 = user;
    conn->status = -1;
    conn->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    return conn;
}
void connectUser(Connection* conn, User* user){
    conn->user2 = user;
    conn->status++;
}

void closeConnection(Connection* conn){
    pthread_mutex_lock(&conn->mutex);
    conn->status++;
    pthread_mutex_unlock(&conn->mutex);

    if(conn->status == 2) free(conn);
}

int isOpen(const Connection* conn){
    return conn->status == 0;
}
