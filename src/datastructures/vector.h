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

    int (*sortfun)(Room*, Room*);
} RoomVector;

// *** Generics Vector functions ***

void initVector(RoomVector*); // Static Default constructor
void freeRooms(RoomVector*); // Static destructor

RoomVector* newVector(); // Dynamic Default constructor
void deleteVector(RoomVector*); // Dynamic destructor

void sortBy(RoomVector*, int (*sortFun)(Room*, Room*));

unsigned int add(RoomVector*, Room*, int autoId);
Room* removeFrom(RoomVector*, unsigned int);

// *** Specific RoomVector functions ***

long indexById(RoomVector*, unsigned int);
Room* getbyId(RoomVector*, unsigned int);
Room* removeById(RoomVector*, unsigned int);

RoomVector* searchByName(RoomVector*, char*); // Must be deleted

RoomVector* RoomVectorCopy(RoomVector*);


#endif //RANDOMCHATSERVER_VECTOR_H
