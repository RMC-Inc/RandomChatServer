#include "server.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


int dispatch(User* usr, RoomVector* vec, int command, char* msg){
    switch (command) {
        case ENTER_IN_ROOM: {

            if (enterInRoom(usr, atoi(msg), vec) == -1)
                return 0;
            break;
        }
        case NEW_ROOM:{
            Room* room = malloc(sizeof(Room));

            sscanf(msg, "%s %d %d.%d.%d %d.%d.%d %d",
                   room->name,
                   &room->icon,
                   &room->iconColor[0], &room->iconColor[1], &room->iconColor[2],
                   &room->roomColor[0], &room->roomColor[1], &room->roomColor[2],
                   &room->time);
            addRoom(room, vec);
        }
            break;
        case ROOM_LIST:
            sendRooms(usr, vec, msg-1);
            break;
        case EXIT:
            return 0;
        default:
            fprintf(stderr, "Unknown cmd: %d %s\n", command, msg);
            return 0;
    }
    return 1;
}


int enterInRoom(User* user , unsigned int id, RoomVector* vec){
    Room* room = getbyId(vec, id);
    if(room == NULL) return -1;

    pthread_mutex_lock(&room->mutex);
    room->usersCount++;
    pthread_mutex_unlock(&room->mutex);

    int next;
    do{
        Connection* conn;
        User* user2 = find(user, room, &conn);
        next = startChatting(user, user2, conn);
        // TODO free connection
    } while (next);

    pthread_mutex_lock(&room->mutex);
    room->usersCount--;
    pthread_mutex_unlock(&room->mutex);

    return 0;
}

void addRoom(Room* room, RoomVector* vec){
    pthread_mutex_lock(&vec->mutex);
    add(vec, room);
    pthread_mutex_unlock(&vec->mutex);
}

void sendRooms(User* user, RoomVector* roomVector, char* buff){
    for (int i = 0; i < roomVector->size; ++i) {
        int len = sprintf(buff, "%s %d %d\n", roomVector->rooms[i]->name, roomVector->rooms[i]->id, roomVector->rooms[i]->usersCount);
        write(user->socketfd, buff, len);
    }
}

/**
 * On Server
 *      rec u1
 *          m -> send u2 m+arg
 *          e | rec error -> send u2 e, exit
 *          n -> send u2 n, skip
 *      if send error -> u2 has exit, send u1 e
 * On client
 *     recv server
 *          m -> display arg
 *          e -> display "user disconnected", if next btn send server n else send e
 *          n -> display "user skipped", send server n or e
 * */

int startChatting(User* userRecv, User* userSend, Connection* conn){ // 0 -> exit; 1 -> next user
    char buff[BUFF_LEN];
    ssize_t len;

    sprintf(buff, "r ok %s\n", userSend->nickname);
    write(userRecv->socketfd, buff, strlen(buff));

    while (1){
        len = read(userRecv->socketfd, buff, BUFF_LEN);
        if(len < 0) { // user exit = recv EXIT
            if(conn->isOpen){
                closeConnection(conn);
                buff[0] = 'e';
                write(userSend->socketfd, buff, 1);
            }
            return 0;
        }

        switch (buff[0]) {
            case SEND_MSG:
                len = write(userSend->socketfd, buff, len);
                if (len < 0) { // second user exit
                    buff[0] = 'e';
                    len = write(userRecv->socketfd, buff, 1);
                    if(len >= 0){
                        len = read(userRecv->socketfd, buff, BUFF_LEN);
                        if(len >= 0 && buff[0] == NEXT_USER) return 1;
                    }
                    return 0;
                }
                break;
            case NEXT_USER:
                if(conn->isOpen){
                    closeConnection(conn);
                    write(userSend->socketfd, buff, len);
                }
                return 1;
            case EXIT:
                if(conn->isOpen){
                    closeConnection(conn);
                    write(userSend->socketfd, buff, len);
                }
                return 0;
        }
    }
}