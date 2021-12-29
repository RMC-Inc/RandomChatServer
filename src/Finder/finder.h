#ifndef RANDOMCHATSERVER_FINDER_H
#define RANDOMCHATSERVER_FINDER_H

#include "../datastructures/entity/user.h"
#include "../datastructures/entity/room.h"
#include <pthread.h>

typedef struct {
    int isOpen;
    pthread_mutex_t mutex;
} Connection;

User* find(User* user, Room*, Connection** conn);
void closeConnection(Connection*);

#endif //RANDOMCHATSERVER_FINDER_H
