#ifndef RANDOMCHATSERVER_SERVER_H
#define RANDOMCHATSERVER_SERVER_H

#include "datastructures/entity/user.h"
#include "datastructures/entity/room.h"
#include "datastructures/vector.h"
#include "Finder/finder.h"


#define PORT 8125
#define BUFF_LEN 500

enum COMMAND{
    // ---- From RoomController ----
    CHANGE_NICKNAME = 'c', // usage: c [new_nick]
    ENTER_IN_ROOM = 'r',
    NEW_ROOM = 'a', // msg pattern: r.g.b icon r.g.b t [roomname]
    ROOM_LIST = 'l', // input: l start end [name]
    // msg pattern: id usercount r.g.b icon r.g.b t [roomname]

    // ---- From ChatController ----
    NEXT_USER = 'n',
    SEND_MSG = 'm',

    TIME_EXPIRED = 't',

    EXIT = 'e',
};

// ---- DISPATCHER ----

int dispatch(User* usr, RoomVector* vec, int command, char* msg); // 1 -> continue; 0 -> exit

// ---- FUNCTIONS ----

int changeNickname(User*, char*); // 1 valid, else 0
void enterInRoom(User*, unsigned int id, RoomVector*, char* buff);
void addRoom(char*, RoomVector*, User*);
void sendRooms(User*, RoomVector*, char*);



int startChatting(User*, User*, Connection*); // Read data from user1 and send to user2 until exit or next
// 0 -> exit
// 1 -> next user


#endif //RANDOMCHATSERVER_SERVER_H