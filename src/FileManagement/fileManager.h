#ifndef RANDOMCHATSERVER_FILEMANAGER_H
#define RANDOMCHATSERVER_FILEMANAGER_H
#include "../datastructures/vector.h"

// return substring len
int stringInside(const char* in, char left, char right, char* out);

void loadFromFile(RoomVector* vec, const char* filename);

pthread_t startAutoSave(RoomVector* vec, const char* filename, int period); // period in seconds


#endif
