#include "server.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>

#include "FileManagement/fileManager.h"


int dispatch(User* usr, RoomVector* vec, int command, char* msg){
    switch (command) {
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
            printf("Unknown cmd: %d %s\n", command, msg);
            return 0;
    }
    return 1;
}

int changeNickname(User* user, char* msg){
    char newNick[NICK_LEN];
    int len = stringInside(msg, '[', ']', newNick, NICK_LEN);

    for (int i = 0; i < len; ++i) {
        if(newNick[i] != ' '){
            strcpy(user->nickname, newNick);
            printf("[%llu] Nickname is set to [%s].\n", user->connectionCount, user->nickname);
            return 1;
        }
    }
    return 0;
}

void enterInRoom(User* user , unsigned int id, RoomVector* vec, char* buff){

    printf("[%llu] Try to enter in room #%d.\n", user->connectionCount, id);

    Room* room = getbyId(vec, id);
    if(room == NULL) {
        printf("[%llu] Error entering in room #%d. Id not valid\n", user->connectionCount, id);
        strcpy(buff, "e\n");
        send(user->socketfd, buff, 2, MSG_NOSIGNAL);
        return;
    }

    pthread_mutex_lock(&room->mutex);
    room->usersCount++;
    pthread_mutex_unlock(&room->mutex);

    int next;
    do{
        printf("[%llu] Searching for user.\n", user->connectionCount);
        Connection* conn = find(user, room, buff-1, BUFF_LEN);
        if(conn == NULL) break;

        User* user2 = (conn->user1 == user)? conn->user2: conn->user1;
        user->prev = user2->connectionCount;

        printf("[%llu] User found, nick: [%s]\n", user->connectionCount, user2->nickname);
        next = startChatting(user, user2, conn, buff-1, room);
    } while (next);

    printf("[%llu] Exit from room #%d.\n", user->connectionCount, id);
    strcpy(buff, "x\n");
    send(user->socketfd, buff, 2, MSG_NOSIGNAL);

    pthread_mutex_lock(&room->mutex);
    room->usersCount--;
    pthread_mutex_unlock(&room->mutex);
}

void addRoom(char* msg, RoomVector* vec, User* user){
    char name[ROOM_NAME_LEN];

    unsigned long long roomColor;
    int t;

    sscanf(msg, "%llu %d",
           &roomColor,
           &t
   );

    stringInside(msg, '[', ']', name, ROOM_NAME_LEN);


    pthread_mutex_lock(&vec->mutex);
    unsigned int id = add(vec, newRoom(name, roomColor, t), 1);
    pthread_mutex_unlock(&vec->mutex);

    printf("[%llu] Room Added {%u %llu %d [%s]}", user->connectionCount, id, roomColor, t, name);

    int len = sprintf(msg, "%c %d\n", NEW_ROOM, id);
    send(user->socketfd, msg, len+1,MSG_NOSIGNAL);
}

void sendRooms(User* user, RoomVector* roomVector, char* buff){

    unsigned int from = 0, to = 0;
    sscanf(buff, "%d %d", &from, &to);

    ssize_t len;
    char name[ROOM_NAME_LEN];
    int nameLen = stringInside(buff, '[', ']', name, ROOM_NAME_LEN);

    RoomVector* source = (nameLen > 0)? searchByName(roomVector, name): RoomVectorCopy(roomVector);

    int sortByUsercount(Room*, Room*);
    sortBy(source, sortByUsercount);

    unsigned int sendingRooms;
    if(from == 0 && to == 0) {
        sendingRooms = source->size;
        to = source->size;
    }else if(to > source->size) sendingRooms = source->size - from;
    else sendingRooms = to - from;

    len = sprintf(buff, "%c %u\n", ROOM_LIST, sendingRooms);
    send(user->socketfd, buff, len, MSG_NOSIGNAL);

    if(source->size != 0){
        printf("[%llu] Sending rooms to client: {\n", user->connectionCount);
        for (; from < to && from < source->size; ++from) {
            Room* r = source->rooms[from];
            len = sprintf(buff, "%c %d %ld %llu %d [%s]\n", ROOM_LIST,
                          r->id,
                          r->usersCount,
                          r->roomColor,
                          r->time,
                          r->name
            );
            len = send(user->socketfd, buff, len, MSG_NOSIGNAL);
            if(len < 0 && errno != EINTR) break;
            printf("[%llu] %s", user->connectionCount, buff);
        }
        printf("[%llu] }\n", user->connectionCount);
    }
    deleteVector(source);
}

int sortByUsercount(Room* a, Room* b){
    return a->usersCount > b->usersCount;
}

int startChatting(User* userRecv, User* userSend, Connection* conn, char* buff, Room* room){ // 0 -> exit; 1 -> next user
    ssize_t len;

    sprintf(buff, "r [%s]\n", userSend->nickname);
    send(userRecv->socketfd, buff, strlen(buff), MSG_NOSIGNAL);


    int timeExpired = 0;
    while (1) {
        fd_set rfdSet, errfdSet;
        FD_SET(userRecv->socketfd, &rfdSet); FD_SET(userRecv->socketfd, &errfdSet);

        if(!isOpen(conn))
            len = select(userRecv->socketfd + 1, &rfdSet, NULL, &errfdSet, NULL);
        else {
            FD_SET(conn->pipefd[0], &rfdSet);
            len = select(((userRecv->socketfd > conn->pipefd[0])? userRecv->socketfd: conn->pipefd[0]) + 1, &rfdSet, NULL, &errfdSet, NULL);
        }
        if(len >= 0){

            if(conn->timer != NULL && !timeExpired && FD_ISSET(conn->pipefd[0], &rfdSet)){ // timer expired
                printf("[%llu] Time expired\n", userRecv->connectionCount);
                buff[0] = TIME_EXPIRED;
                buff[1] = '\n';
                send(userRecv->socketfd, buff, 2, MSG_NOSIGNAL);
                timeExpired = 1;
            }

            if(FD_ISSET(userRecv->socketfd, &rfdSet) || FD_ISSET(userRecv->socketfd, &errfdSet)){
                len = recv(userRecv->socketfd, buff, BUFF_LEN, MSG_NOSIGNAL);
                if(len <= 0) { // user close socket === recv EXIT
                    if(errno == EINTR) continue;
                    if(isOpen(conn)){
                        closeConnection(conn);
                        buff[0] = EXIT;
                        buff[1] = '\n';
                        send(userSend->socketfd, buff, 2, MSG_NOSIGNAL);
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

                    case USERS_IN_ROOM:
                        len = sprintf(buff, "%c %lu\n", USERS_IN_ROOM, room->usersCount);
                        send(userRecv->socketfd, buff, len, MSG_NOSIGNAL);
                        break;
                    case EXIT:
                        if(isOpen(conn)){
                            closeConnection(conn);
                            send(userSend->socketfd, buff, len, MSG_NOSIGNAL);
                        } else closeConnection(conn);
                        return 0;
                }
            }
        }

    }
}