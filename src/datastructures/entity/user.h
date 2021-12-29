//
// Created by matte on 21/12/2021.
//

#ifndef RANDOMCHATSERVER_USER_H
#define RANDOMCHATSERVER_USER_H

#define NICK_LEN 20


typedef struct USR{
    char nickname[NICK_LEN];
    int socketfd;
} User;

#endif //RANDOMCHATSERVER_USER_H
