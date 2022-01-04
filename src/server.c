#include "server.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


int dispatch(User* usr, RoomVector* vec, int command, char* msg){
    switch (command) {
        case ENTER_IN_ROOM:
            enterInRoom(usr, atoi(msg), vec);
            return 0;
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


void enterInRoom(User* user , unsigned int id, RoomVector* vec){
    Room* room = getbyId(vec, id);
    if(room == NULL) return;

    pthread_mutex_lock(&room->mutex);
    room->usersCount++;
    pthread_mutex_unlock(&room->mutex);

    int next;
    do{
        Connection* conn = find(user, room);
        User* user2 = (conn->user1 == user)? conn->user2: conn->user1;
        next = startChatting(user, user2, conn);
    } while (next);

    pthread_mutex_lock(&room->mutex);
    room->usersCount--;
    pthread_mutex_unlock(&room->mutex);
}

void addRoom(Room* room, RoomVector* vec){
    pthread_mutex_lock(&vec->mutex);
    add(vec, room);
    pthread_mutex_unlock(&vec->mutex);
}

void sendRooms(User* user, RoomVector* roomVector, char* buff){
    for (int i = 0; i < roomVector->size; ++i) {
        int len = sprintf(buff, "%s %d %ld\n", roomVector->rooms[i]->name, roomVector->rooms[i]->id, roomVector->rooms[i]->usersCount);
        write(user->socketfd, buff, len);
    }
}

int startChatting(User* userRecv, User* userSend, Connection* conn){ // 0 -> exit; 1 -> next user
    char buff[BUFF_LEN];
    ssize_t len;

    sprintf(buff, "r %s\n", userSend->nickname);
    write(userRecv->socketfd, buff, strlen(buff));

    while (1) {
        len = read(userRecv->socketfd, buff, BUFF_LEN);
        if(len < 0) { // user close socket === recv EXIT
            if(isOpen(conn)){
                closeConnection(conn);
                buff[0] = 'e';
                write(userSend->socketfd, buff, 1);
            } else closeConnection(conn);
            return 0;
        }

        switch (buff[0]) {
            case SEND_MSG:
                if(isOpen(conn)){
                    write(userSend->socketfd, buff, len);
                }
                break;
            case NEXT_USER:
                if(isOpen(conn)){
                    closeConnection(conn);
                    write(userSend->socketfd, buff, len);
                } else closeConnection(conn);
                return 1;
            case EXIT:
                if(isOpen(conn)){
                    closeConnection(conn);
                    write(userSend->socketfd, buff, len);
                }
                return 0;
        }
    }
}