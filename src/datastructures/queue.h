#ifndef RANDOMCHATSERVER_QUEUE_H
#define RANDOMCHATSERVER_QUEUE_H

#include "entity/user.h"

struct UserNode{
    User* data;
    struct UserNode* next;
};


typedef struct {
    struct UserNode* front;
    struct UserNode* rear;
    unsigned int size;
} UserQueue;

void newUserQueue(UserQueue*); // default constructor

User* top(UserQueue);
void enqueue(UserQueue*, User*);
User* dequeue(UserQueue*);

#endif //RANDOMCHATSERVER_QUEUE_H
