#include "server.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "FileManagement/fileManager.h"


int dispatch(User* usr, RoomVector* vec, int command, char* msg){
    switch (command) {
        case ENTER_IN_ROOM:
            enterInRoom(usr, atoi(msg), vec);
            return 0;
        case NEW_ROOM:
            addRoom(msg, vec);
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
        user->prev_tid = user2->tid;

        next = startChatting(user, user2, conn);
    } while (next);

    pthread_mutex_lock(&room->mutex);
    room->usersCount--;
    pthread_mutex_unlock(&room->mutex);
}

void addRoom(char* msg, RoomVector* vec){
    char name[ROOM_NAME_LEN];

    int icon;
    int iconColor[3], roomColor[3];
    int t;

    sscanf(msg, "%d.%d.%d %d %d.%d.%d %d",
           &roomColor[0], &roomColor[1], &roomColor[2],
           &icon,
           &iconColor[0], &iconColor[1], &iconColor[2],
           &t
   );

    unsigned char iconC[3] = {(unsigned char) iconColor[0], (unsigned char) iconColor[2], (unsigned char) iconColor[3]};
    unsigned char roomC[3] = {(unsigned char) roomColor[0], (unsigned char) roomColor[2], (unsigned char) roomColor[3]};

    stringInside(msg, '[', ']', name);

    pthread_mutex_lock(&vec->mutex);
    add(vec, newRoom(name, icon, iconC, roomC, t));
    pthread_mutex_unlock(&vec->mutex);
}

void sendRooms(User* user, RoomVector* roomVector, char* buff){
    int len = sprintf(buff, "%d\n", roomVector->size);
    write(user->socketfd, buff, len);
    for (int i = 0; i < roomVector->size; ++i) {
        len = sprintf(buff, "%d %ld %d.%d.%d %d %d.%d.%d %d [%s]\n",
                          roomVector->rooms[i]->id,
                          roomVector->rooms[i]->usersCount,
                          roomVector->rooms[i]->roomColor[0], roomVector->rooms[i]->roomColor[1], roomVector->rooms[i]->roomColor[2],
                          roomVector->rooms[i]->icon,
                          roomVector->rooms[i]->iconColor[0], roomVector->rooms[i]->iconColor[1], roomVector->rooms[i]->iconColor[2],
                          roomVector->rooms[i]->time,
                          roomVector->rooms[i]->name
        );
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
                buff[0] = EXIT;
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
                } else closeConnection(conn);
                return 0;
        }
    }
}