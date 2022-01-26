#ifndef RANDOMCHATSERVER_FINDER_H
#define RANDOMCHATSERVER_FINDER_H

#include "../datastructures/entity/user.h"
#include "../datastructures/entity/room.h"
#include <pthread.h>
#include "connection.h"


Connection* find(User* user, Room*, char* buff, int buff_size); // Blocking


#endif //RANDOMCHATSERVER_FINDER_H
