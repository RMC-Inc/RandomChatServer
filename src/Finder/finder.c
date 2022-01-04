#include "finder.h"
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "../datastructures/queue.h"



// TODO free connection


// *** Finder ***

void signHandler(int signal){ return; }

Connection* find(User* user, Room* room){
    pthread_mutex_lock(&room->mutex);
    if(room->waitlist.size == 0 /*TODO || top == prevUser*/){
        Connection* conn = createConnection(user);

        enqueue(&room->waitlist, conn);

        signal(SIGUSR1, signHandler);

        pthread_mutex_unlock(&room->mutex);

        while (conn->user2 == NULL) pause();

        return conn;
    } else {
        Connection* conn = (Connection*) dequeue(&room->waitlist);
        connectUser(conn, user);

        pthread_kill(conn->user1->tid, SIGUSR1);

        pthread_mutex_unlock(&room->mutex);

        return conn;
    }
}

/*
 * Room contiene una waitlist di connection
 *
 * utente chiama find
 *      mutex
 *      se waitlist è vuota || il top è l'utente precedente Crea una connessione e la mette in waitlist rilascia il mutex, pause()
 *      se non è vuota
 *          estrae l'utente dalla waitlis, setta connection e usa kill per avvertirlo
 *          rilascia il mutex return connection
 *      dopo il pause se connection is open return connection
 *
 *
 * **/
