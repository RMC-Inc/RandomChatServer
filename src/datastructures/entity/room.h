//
// Created by matte on 21/12/2021.
//

#ifndef RANDOMCHATSERVER_ROOM_H
#define RANDOMCHATSERVER_ROOM_H

#include <pthread.h>
#include "../queue.h"

typedef struct {
    char name[30];
    unsigned int id; // autoincrement

    int icon;
    char iconColor[3];
    char roomColor[3];

    unsigned int time; // in seconds

    pthread_mutex_t mutex;
    UserQueue waitqueue;
} Room;

#endif //RANDOMCHATSERVER_ROOM_H
