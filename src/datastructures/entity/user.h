//
// Created by matte on 21/12/2021.
//

#ifndef RANDOMCHATSERVER_USER_H
#define RANDOMCHATSERVER_USER_H


typedef struct {
    char nickname[20];
    int socketDes;
} User;

typedef struct {
    User* first;
    User* second;
} Pair;

#endif //RANDOMCHATSERVER_USER_H
