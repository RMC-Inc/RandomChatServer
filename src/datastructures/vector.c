#include "vector.h"
#include <ctype.h>
#include <string.h>


void Expand(RoomVector* vec){ // private
    if (vec->size >= vec->realSize-1){
        vec->realSize *= RESIZE_FACTOR;
        vec->rooms = realloc(vec->rooms, sizeof(Room*) * vec->realSize);
    }
}

void Reduce(RoomVector* vec){ // private
    unsigned int newSize = vec->realSize/RESIZE_FACTOR;
    if (vec->size <= vec->realSize/4 && newSize > vec->size && newSize >= INIT_SIZE){
        vec->rooms = realloc(vec->rooms, sizeof(Room*) * newSize);
        vec->realSize = newSize;
    }
}

void initVector(RoomVector* vec){
    vec->size = 0;
    vec->realSize = INIT_SIZE;

    vec->rooms = malloc(sizeof(Room*) * vec->realSize);

    vec->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
}

RoomVector* newVector(){
    RoomVector* vec = malloc(sizeof(RoomVector));
    initVector(vec);
    return vec;
}


void freeRooms(RoomVector* vec){
    free(vec->rooms);

}

void deleteVector(RoomVector* vec){
    freeRooms(vec);
    free(vec);
}


void insertionSort(RoomVector* v){ // private
    for (unsigned int i = 1; i < v->size; ++i) {
        unsigned int j = i;
        while (j > 0 && v->rooms[j]->id < v->rooms[j-1]->id){
            Room* tmp = v->rooms[j];
            v->rooms[j] = v->rooms[j-1];
            v->rooms[j-1] = tmp;
            j--;
        }
    }
}

unsigned int nextAvailableId(RoomVector* vec){ // private
    unsigned int id = 0;
    unsigned i = 0;
    while (i < vec->size && id == vec->rooms[i]->id) {
        id++;
        i++;
    }
    return id;
}

unsigned int add(RoomVector* vec, Room* room){
    room->id = nextAvailableId(vec);
    vec->rooms[vec->size++] = room;
    insertionSort(vec);
    Expand(vec);
    return room->id;
}

Room* removeFrom(RoomVector* vec, unsigned int index){
    if(vec->size == 0 || index >= vec->size) return NULL;
    Room* ret = vec->rooms[index];

    while (index < vec->size - 1){
        vec->rooms[index] = vec->rooms[index+1];
        index++;
    }
    vec->size--;

    Reduce(vec);
    return ret;
}

long indexById(RoomVector* vec, unsigned int id){
    if(vec->size == 0) return -1;
    long l = 0, r = vec->size - 1;

    while (l <= r){
        long m = (l+r)/2;
        unsigned int mID = vec->rooms[m]->id;
        if(mID == id) return m;
        else if(id < mID) r = m-1;
        else l = m+1;
    }
    return -1;
}

Room* getbyId(RoomVector* vec, unsigned int id){
    long i = indexById(vec, id);
    if(i == -1) return NULL;
    else return vec->rooms[i];
}

Room* removeById(RoomVector* vec, unsigned int id){
    long i = indexById(vec, id);
    if(i == -1) return NULL;
    else return removeFrom(vec, i);
}

void strLowercase(char* out){ // private
    for (; *out != '\0'; ++out) *out = tolower(*out);
}

RoomVector* searchByName(RoomVector* in, char* name){
    RoomVector* out = newVector();
    strLowercase(name);
    char roomName[ROOM_NAME_LEN];

    for (int i = 0; i < in->size; ++i) {
        strcpy(roomName, in->rooms[i]->name);
        strLowercase(roomName);
        if(strstr(roomName, name)){
            add(out, in->rooms[i]);
        }
    }

    return out;
}







