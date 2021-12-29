#include "finder.h"
#include <unistd.h>
#include <stdlib.h>
#include "../datastructures/queue.h"

Connection* cn;
int count = 0;

User* find(User* user, Room* room, Connection** conn){
    if(room->waitlist.size == 0){
        cn = malloc(sizeof (Connection));
        cn->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
        cn->isOpen = 1;
    }
    *conn = cn;
    enqueue(&room->waitlist, user);
    while (room->waitlist.size == 1)
        sleep(2);
    if(user == top(room->waitlist)){
        User* ret = room->waitlist.front->next->data;
        count++;
        if(count == 2)
        {
            while (room->waitlist.size > 0) dequeue(&room->waitlist);
            count = 0;
        }
        return ret;
    }
    else {
        count++;
        if(count == 2)
        {
            while (room->waitlist.size > 0) dequeue(&room->waitlist);
            count = 0;
        }
        return top(room->waitlist);
    }
}


void closeConnection(Connection* conn){
    pthread_mutex_lock(&conn->mutex);
    conn->isOpen = 0;
    pthread_mutex_unlock(&conn->mutex);
}