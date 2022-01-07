#ifndef RANDOMCHATSERVER_QUEUE_H
#define RANDOMCHATSERVER_QUEUE_H

#include "entity/user.h"

struct QueueNode{
    void* data;
    struct QueueNode* next;
};


typedef struct {
    struct QueueNode* front;
    struct QueueNode* rear;
    unsigned int size;
} Queue;

void newQueue(Queue*); // default constructor

void* top(Queue*);
void enqueue(Queue*, void*);
void* dequeue(Queue*);

#endif //RANDOMCHATSERVER_QUEUE_H
