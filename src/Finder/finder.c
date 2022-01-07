#include "finder.h"
#include <unistd.h>
#include <signal.h>

#include <pthread.h>


// TODO free connection


// *** Finder ***

void signHandler(int sig){ return; }

Connection* find(User* user, Room* room){
    pthread_mutex_lock(&room->mutex);
    pthread_t tid = -1;
    if(room->waitlist.size != 0){
        tid = ((Connection*) top(&room->waitlist))->user1->tid;
    }
    if(room->waitlist.size == 0 ||
    (tid != -1 && pthread_equal(user->prev_tid, tid))){
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
