#include "fileManager.h"
#include <string.h>
#include <stdio.h>
#include "../Timer/timer.h"

int stringInside(const char* in, char left, char right, char* out, int maxLen){
    int strStart, strEnd;
    unsigned int strLen = strlen(in);

    for (strStart = 0; strStart < strLen && in[strStart] != left; ++strStart);
    strStart++;
    for (strEnd = strStart; strEnd < strLen && in[strEnd] != right; ++strEnd);
    if(strEnd <= strStart || strEnd - strStart >= maxLen) return 0;

    memcpy(out,in + strStart, strEnd - strStart);
    out[strEnd - strStart] = '\0';

    return strEnd - strStart;
}


void loadFromFile(RoomVector* vec, const char* filename){
    pthread_mutex_lock(&vec->mutex);
    FILE* file = fopen(filename, "r");
    if(file != NULL){

        char* roomStr = NULL;
        size_t strLen = 0;

        int id;
        unsigned long long roomColor;
        int time;
        char name[ROOM_NAME_LEN];

        while (getline(&roomStr, &strLen, file) != -1){
            sscanf(roomStr, "%d %llu %d",
                    &id,
                    &roomColor,
                    &time
            );
            stringInside(roomStr, '[', ']', name, ROOM_NAME_LEN);
            printf("Adding room: %d %llu %d [%s]\n",
                    id,
                    roomColor,
                    time,
                    name
            );
            Room* room = newRoom(name, roomColor, time);
            room->id = id;
            add(vec, room, 0);
        }

        free(roomStr);
        fclose(file);
    }
    pthread_mutex_unlock(&vec->mutex);
}


struct autoSaveArgs{
    RoomVector* vec;
    char filename[30];
    int period;
};

_Noreturn void* autoSaveThread(void* args){
    struct autoSaveArgs* arg = (struct autoSaveArgs*) args;

    RoomVector* vec = arg->vec;
    const char* filename = arg->filename;
    int period = arg->period;

    while (1){
        Timer* t = newTimer(period, 0);
        startTimer(t);
        waitForTimer(t);

        pthread_mutex_lock(&vec->mutex);
        FILE* file = fopen(filename, "w");
        if(file != NULL){
            for (int i = 0; i < vec->size; ++i) {
                Room* r = vec->rooms[i];
                fprintf(file, "%d %llu %d [%s]\n",
                      r->id,
                      r->roomColor,
                      r->time,
                      r->name
                );
            }
            fclose(file);
        }
        pthread_mutex_unlock(&vec->mutex);
        deleteTimer(t);
    }
}


pthread_t startAutoSave(RoomVector* vec, const char* filename, int period){
    struct autoSaveArgs* args = malloc(sizeof(struct autoSaveArgs));
    args->vec = vec;
    strcpy(args->filename, filename);
    args->period = period;

    pthread_t tid;
    pthread_create(&tid, NULL, autoSaveThread, (void*) args);
    return tid;
}