//
// Created by matte on 21/12/2021.
//

#ifndef RANDOMCHATSERVER_ROOM_H
#define RANDOMCHATSERVER_ROOM_H

#include <pthread.h>
#include "../queue.h"

typedef struct {
    char name[31];
    unsigned int id; // unique

    int icon;
    char iconColor[3];
    char roomColor[3];

    unsigned int time; // in seconds

    pthread_mutex_t mutex;
    Queue waitlist;
    long usersCount;
} Room;

Room* newRoom(char* name, unsigned int id, int icon, char* iColor, char* rColor, unsigned int time);


#endif //RANDOMCHATSERVER_ROOM_H
