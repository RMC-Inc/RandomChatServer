#ifndef RANDOMCHATSERVER_VECTOR_H
#define RANDOMCHATSERVER_VECTOR_H

#include <stdlib.h>
#include <pthread.h>
#include "entity/room.h"
#include "entity/user.h"

#define INIT_SIZE 4
#define RESIZE_FACTOR 2


typedef struct {
    unsigned int size;
    unsigned int realSize;
    Room** rooms;

    pthread_mutex_t mutex;
    unsigned int nullCount;
} RoomVector;

void newVector(RoomVector*); // Default constructor

void add(RoomVector*, Room*);
Room* removeFrom(RoomVector*, unsigned int);

long indexById(RoomVector*, unsigned int);
Room* getbyId(RoomVector*, unsigned int);
Room* removeById(RoomVector*, unsigned int);
unsigned int nextId(RoomVector*); // TODO


#endif //RANDOMCHATSERVER_VECTOR_H
