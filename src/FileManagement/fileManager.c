#include "fileManager.h"
#include <string.h>
#include <stdio.h>
#include "../Timer/timer.h"

int stringInside(const char* in, char left, char right, char* out){
    int strStart, strEnd;
    unsigned int strLen = strlen(in);

    for (strStart = 0; strStart < strLen && in[strStart] != '['; ++strStart);
    strStart++;
    for (strEnd = strStart; strEnd < strLen && in[strEnd] != ']'; ++strEnd);
    if(strEnd <= strStart) return 0;

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
        int roomColor[3], iconColor[3];
        int icon;
        int time;
        char name[ROOM_NAME_LEN];

        while (getline(&roomStr, &strLen, file) != -1){
            sscanf(roomStr, "%d %d.%d.%d %d %d.%d.%d %d",
                    &id,
                    &roomColor[0], &roomColor[1], &roomColor[2],
                    &icon,
                    &iconColor[0], &iconColor[1], &iconColor[2],
                    &time
            );
            stringInside(roomStr, '[', ']', name);
            printf("Adding room: %d %d.%d.%d %d %d.%d.%d %d [%s]\n",
                    id,
                    roomColor[0], roomColor[1], roomColor[2],
                    icon,
                    iconColor[0], iconColor[1], iconColor[2],
                    time,
                    name
            );
            unsigned char iColor[3] = {(unsigned char) iconColor[0], (unsigned char) iconColor[1], (unsigned char) iconColor[2]};
            unsigned char rColor[3] = {(unsigned char) roomColor[0], (unsigned char) roomColor[1], (unsigned char) roomColor[2]};
            Room* room = newRoom(name, icon, iColor, rColor, time);
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
                fprintf(file, "%d %d.%d.%d %d %d.%d.%d %d [%s]\n",
                      r->id,
                      r->roomColor[0], r->roomColor[1], r->roomColor[2],
                      r->icon,
                      r->iconColor[0], r->iconColor[1], r->iconColor[2],
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
}