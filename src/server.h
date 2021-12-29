#ifndef RANDOMCHATSERVER_SERVER_H
#define RANDOMCHATSERVER_SERVER_H

#include "datastructures/entity/user.h"
#include "datastructures/entity/room.h"
#include "datastructures/vector.h"
#include "Finder/finder.h"


#define PORT 8080
#define BUFF_LEN 500

enum COMMAND{
    // ---- From RoomController ----
    ENTER_IN_ROOM = 'r',
    NEW_ROOM = 'a',
    ROOM_LIST = 'l',

    // ---- From ChatController ----
    NEXT_USER = 'n',
    SEND_MSG = 'm',

    EXIT = 'e',
};

// ---- DISPATCHER ----

int dispatch(User* usr, RoomVector* vec, int command, char* msg); // 1 -> continue; 0 -> exit

// ---- FUNCTIONS ----

int enterInRoom(User*, unsigned int id, RoomVector*); // Find another user. 1 ok -1 error
void addRoom(Room*, RoomVector*);
void sendRooms(User*, RoomVector*, char*);



int startChatting(User*, User*, Connection*); // Read data from user1 and send to user2 until exit or next
// 0 -> exit
// 1 -> next user


#endif //RANDOMCHATSERVER_SERVER_H
