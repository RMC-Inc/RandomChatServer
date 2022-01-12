#include "server.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "FileManagement/fileManager.h"


int dispatch(User* usr, RoomVector* vec, int command, char* msg){
    switch (command) {
        case DELETE_ROOM:
            deleteRoom(vec, msg);
            break;
        case CHANGE_NICKNAME:
            changeNickname(usr, msg);
            break;
        case ENTER_IN_ROOM:
            enterInRoom(usr, atoi(msg), vec, msg);
            break;
        case NEW_ROOM:
            addRoom(msg, vec, usr);
            break;
        case ROOM_LIST:
            sendRooms(usr, vec, msg);
            break;
        case EXIT:
            return 0;
        default:
            fprintf(stderr, "Unknown cmd: %d %s\n", command, msg);
            return 0;
    }
    return 1;
}


void deleteRoom(RoomVector* vec, char* msg){ //TODO
    int roomId;
    sscanf(msg, "%d", &roomId);
    printf("Debug room id: %d\n", roomId);
}

int changeNickname(User* user, char* msg){
    char newNick[NICK_LEN];
    int len = stringInside(msg, '[', ']', newNick);

    // controllo ridondante, è proprio necessario?? Ricorda gli hacker russi... Comunque così è anche più efficiente di strcmp
    for (int i = 0; i < len; ++i) {
        if(newNick[i] != ' '){
            strcpy(user->nickname, newNick);
            printf("[%lu] Nickname is set to [%s].\n", pthread_self(), user->nickname);
            return 1;
        }
    }
    return 0;
}

void enterInRoom(User* user , unsigned int id, RoomVector* vec, char* buff){

    printf("[%lu] Try to enter in room #%d.\n", pthread_self(), id);

    Room* room = getbyId(vec, id);
    if(room == NULL) {
        printf("[%lu] Error entering in room #%d. Id not valid\n", pthread_self(), id);
        strcpy(buff, "e\n");
        send(user->socketfd, buff, 2, MSG_NOSIGNAL);
        return;
    }

    pthread_mutex_lock(&room->mutex);
    room->usersCount++;
    pthread_mutex_unlock(&room->mutex);

    int next;
    do{
        printf("[%lu] Searching for user.\n", pthread_self());
        Connection* conn = find(user, room);
        if(conn == NULL) break;

        User* user2 = (conn->user1 == user)? conn->user2: conn->user1;
        user->prev_tid = user2->tid;

        printf("[%lu] User found, nick: [%s]\n", pthread_self(), user2->nickname);
        next = startChatting(user, user2, conn);
    } while (next);

    printf("[%lu] Exit from room #%d.\n", pthread_self(), id);
    strcpy(buff, "e\n");
    send(user->socketfd, buff, 2, MSG_NOSIGNAL);

    pthread_mutex_lock(&room->mutex);
    room->usersCount--;
    pthread_mutex_unlock(&room->mutex);
}

void addRoom(char* msg, RoomVector* vec, User* user){
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

    unsigned char iconC[3] = {(unsigned char) iconColor[0], (unsigned char) iconColor[1], (unsigned char) iconColor[2]};
    unsigned char roomC[3] = {(unsigned char) roomColor[0], (unsigned char) roomColor[1], (unsigned char) roomColor[2]};


    stringInside(msg, '[', ']', name);


    pthread_mutex_lock(&vec->mutex);
    unsigned int id = add(vec, newRoom(name, icon, iconC, roomC, t));
    pthread_mutex_unlock(&vec->mutex);

    int len = sprintf(msg, "%c %d\n", NEW_ROOM, id);
    send(user->socketfd, msg, len+1,MSG_NOSIGNAL);
}

void sendRooms(User* user, RoomVector* roomVector, char* buff){

    unsigned int size;
    sscanf(buff, "%d", &size);

    int len;
    char name[ROOM_NAME_LEN];
    int nameLen = stringInside(buff, '[', ']', name);

    RoomVector* search;
    if(nameLen > 0)
        search = searchByName(roomVector, name);
    RoomVector* source = (nameLen > 0)? search: roomVector;

    if(source->size < size) size = source->size;

    // TODO inviare in ordine decrescente di usercount

    char* tmp = buff;
    for (int i = 0; i < size && size <= source->size; ++i) {
        Room* r = source->rooms[i];
        len = sprintf(tmp, "%d %ld %d.%d.%d %d %d.%d.%d %d [%s]\n",
                      r->id,
                      r->usersCount,
                      r->roomColor[0], r->roomColor[1], r->roomColor[2],
                      r->icon,
                      r->iconColor[0], r->iconColor[1], r->iconColor[2],
                      r->time,
                      r->name
        );
        tmp += len;
    }
    printf("[%lu] Sending rooms to client {\n%s}\n", pthread_self(), buff);
    send(user->socketfd, buff, tmp-buff, MSG_NOSIGNAL);
    if (nameLen > 0) deleteVector(search);
}

int startChatting(User* userRecv, User* userSend, Connection* conn){ // 0 -> exit; 1 -> next user
    char buff[BUFF_LEN];
    ssize_t len;

    sprintf(buff, "r [%s]\n", userSend->nickname);
    send(userRecv->socketfd, buff, strlen(buff), MSG_NOSIGNAL);

    while (1) {
        len = recv(userRecv->socketfd, buff, BUFF_LEN, MSG_NOSIGNAL);
        if(len <= 0) { // user close socket === recv EXIT
            if(isOpen(conn)){
                closeConnection(conn);
                buff[0] = EXIT;
                send(userSend->socketfd, buff, 1, MSG_NOSIGNAL);
            } else closeConnection(conn);
            return 0;
        }

        switch (buff[0]) {
            case SEND_MSG:
                if(isOpen(conn)){
                    send(userSend->socketfd, buff, len, MSG_NOSIGNAL);
                }
                break;
            case NEXT_USER:
                if(isOpen(conn)){
                    closeConnection(conn);
                    send(userSend->socketfd, buff, len, MSG_NOSIGNAL);
                } else closeConnection(conn);
                return 1;
            case EXIT:
                if(isOpen(conn)){
                    closeConnection(conn);
                    send(userSend->socketfd, buff, len, MSG_NOSIGNAL);
                } else closeConnection(conn);
                return 0;
        }
    }
}