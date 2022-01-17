#ifndef RANDOMCHATSERVER_CONNECTION_H
#define RANDOMCHATSERVER_CONNECTION_H
#include <pthread.h>

#include "../datastructures/entity/user.h"
#include "../Timer/timer.h"

// *** Connection ***
typedef struct {
    User* user1;
    User* user2;

    int status;
    pthread_mutex_t mutex;
    int pipefd[2];
    Timer* timer;
} Connection;

Connection* createConnection(User*);
void setConnectionTimeout(Connection*, unsigned int timeout);
void connectUser(Connection*, User*);
void closeConnection(Connection*);
int isOpen(const Connection*);

#endif //RANDOMCHATSERVER_CONNECTION_H
