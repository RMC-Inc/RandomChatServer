#include "finder.h"
#include <unistd.h>
#include <stdlib.h>

#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <stdio.h>

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
    printf("[%ld][FINDER] Queue Size: %d\n", pthread_self(), room->waitlist.size);
    if(room->waitlist.size == 0 ||
    (tid != -1 && pthread_equal(user->prev_tid, tid))){
        printf("[%ld][FINDER] Enter in waitlist\n", pthread_self());
        Connection* conn = createConnection(user);

        enqueue(&room->waitlist, conn);

        signal(SIGUSR1, signHandler); // todo reset handler

        pthread_mutex_unlock(&room->mutex);

        while (conn->user2 == NULL){
            fd_set rfdSet; // TODO errorFdSet
            FD_ZERO(&rfdSet);
            FD_SET(user->socketfd, &rfdSet);

            int retVal = select(user->socketfd + 1, &rfdSet, NULL, NULL, NULL);
            if(retVal >= 0){
                if(FD_ISSET(user->socketfd, &rfdSet)){
                    char buff[10];
                    unsigned int len = recv(user->socketfd, buff, 10, MSG_DONTWAIT);
                    if(len > 0){
                        buff[len] = '\0';
                        printf("[%ld][FINDER] Recived message: %s\n", pthread_self(), buff);
                    }

                    if(len > 0 && buff[0] == 'e'){
                        printf("[%ld][FINDER] Try extract connection from waitlist\n", pthread_self());
                        pthread_mutex_lock(&room->mutex);
                        void* removeRet = extract(&room->waitlist, conn);
                        pthread_mutex_unlock(&room->mutex);

                        if(removeRet == NULL){ // Connection extract by other user
                            printf("[%ld][FINDER] Connection not found in waitlist closing connection\n", pthread_self());
                            if(isOpen(conn)){
                                closeConnection(conn);
                                buff[1] = '\n';
                                write(conn->user2->socketfd, buff, 2);
                            } else closeConnection(conn);
                        } else {
                            printf("[%ld][FINDER] Connection removed\n", pthread_self());
                            free(conn); // double close not necessary
                        }
                        return NULL;
                    }
                }
            }
        }

        return conn;
    } else {
        printf("[%ld][FINDER] Extract user from waitlist\n", pthread_self());
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
