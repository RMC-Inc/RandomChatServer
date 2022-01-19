#include "room.h"
#include <stdlib.h>
#include <string.h>

Room* newRoom(char* name, unsigned long long rColor, unsigned int time){
    Room* room = malloc(sizeof (Room));
    strcpy(room->name, name);
    room->roomColor = rColor;
    room->time = time;

    room->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    newQueue(&room->waitlist);
    return room;
}
