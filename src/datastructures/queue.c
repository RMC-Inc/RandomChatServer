#include "queue.h"
#include <stdlib.h>


void newQueue(Queue* queue){
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
}

void* top(Queue* queue){
    if(queue->size == 0) return NULL;
    return queue->front->data;
}

// front -> data -> data <- rear
void enqueue(Queue* queue,  void* data){
    struct QueueNode* new = malloc(sizeof(struct QueueNode));
    new->data = data;
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

void* dequeue(Queue* queue){
    if(queue->size > 0){
        User* ret = queue->front->data;

        struct QueueNode* del = queue->front;
        queue->front = queue->front->next;

        if(queue->size == 1) queue->rear = NULL;

        free(del);
        queue->size--;
        return ret;
    }
    return NULL;
}

void* extract(Queue* queue, void* data){
    if(queue->size != 0){
        if(data == queue->front->data){
            return dequeue(queue);
        } else {
            struct QueueNode* tmp = queue->front;
            while (tmp->next != NULL){
                if(tmp->next->data == data){
                    struct QueueNode* del = tmp->next;
                    tmp->next = del->next;
                    if(queue->rear == del) queue->rear = tmp;
                    free(del);
                    queue->size--;
                    return data;
                }
            }
        }
    }
    return NULL;
}