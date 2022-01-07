//
// Created by matte on 21/12/2021.
//

#ifndef RANDOMCHATSERVER_ROOM_H
#define RANDOMCHATSERVER_ROOM_H

#include <pthread.h>
#include "../queue.h"

#define ROOM_NAME_LEN 31

typedef struct {
    char name[ROOM_NAME_LEN];
    unsigned int id; // unique

    int icon;
    unsigned char iconColor[3];
    unsigned char roomColor[3];

    unsigned int time; // in seconds

    pthread_mutex_t mutex;
    Queue waitlist;
    long usersCount;
} Room;

Room* newRoom(char* name, int icon, unsigned char* iColor, unsigned char* rColor, unsigned int time);


#endif //RANDOMCHATSERVER_ROOM_H
