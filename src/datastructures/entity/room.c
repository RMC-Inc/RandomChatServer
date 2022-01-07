#include "room.h"
#include <stdlib.h>
#include <string.h>

Room* newRoom(char* name, int icon, unsigned char* iColor, unsigned char* rColor, unsigned int time){
    Room* room = malloc(sizeof (Room));
    strcpy(room->name, name);
    room->icon = icon;
    memcpy(room->iconColor, iColor, 3);
    memcpy(room->roomColor, rColor, 3);
    room->time = time;

    room->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    newQueue(&room->waitlist);
    return room;
}
