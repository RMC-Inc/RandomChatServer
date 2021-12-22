#include "queue.h"
#include <stdlib.h>


void newUserQueue(UserQueue* queue){
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
}

User* top(UserQueue queue){
    if(queue.size == 0) return NULL;
    return queue.front->data;
}

// front -> data -> data <- rear
void enqueue(UserQueue* queue,  User* user){
    struct UserNode* new = malloc(sizeof(struct UserNode));
    new->data = user;
    new->next = NULL;

    if(queue->size == 0){
        queue->front = new;
        queue->rear = new;
    } else {
        queue->rear->next = new;
        queue->rear = new;
    }
    queue->size++;
}
User* dequeue(UserQueue* queue){
    if(queue->size > 0){
        User* ret = queue->front->data;

        struct UserNode* del = queue->front;
        queue->front = queue->front->next;

        if(queue->size == 1) queue->rear = NULL;

        free(del);
        queue->size--;
        return ret;
    }
    return NULL;
}