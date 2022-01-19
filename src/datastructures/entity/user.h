//
// Created by matte on 21/12/2021.
//

#ifndef RANDOMCHATSERVER_USER_H
#define RANDOMCHATSERVER_USER_H

#define NICK_LEN 21

#include <pthread.h>


typedef struct USR{
    char nickname[NICK_LEN];
    int socketfd;

    struct USR* prevUser;
} User;

#endif //RANDOMCHATSERVER_USER_H
