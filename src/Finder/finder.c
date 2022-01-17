#include "finder.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>


// *** Finder ***

Connection* find(User* user, Room* room) {
    pthread_mutex_lock(&room->mutex);

    pthread_t tid = -1;
    if (room->waitlist.size != 0) {
        tid = ((Connection *) top(&room->waitlist))->user1->tid;
    }
    printf("[%ld][FINDER] Queue Size: %d\n", pthread_self(), room->waitlist.size);
    if (room->waitlist.size == 0 ||
        (tid != -1 && pthread_equal(user->prev_tid, tid))) { // TODO deve vedere se c'è qualcuno dopo al top
        printf("[%ld][FINDER] Enter in waitlist\n", pthread_self());
        Connection *conn = createConnection(user);

        enqueue(&room->waitlist, conn);

        pthread_mutex_unlock(&room->mutex);

        while (conn->user2 == NULL) {
            fd_set rfdSet, errfdSet;
            FD_ZERO(&rfdSet);
            FD_ZERO(&errfdSet);
            FD_SET(user->socketfd, &rfdSet);
            FD_SET(user->socketfd, &errfdSet);
            FD_SET(conn->pipefd[0], &rfdSet);

            int retVal = select(((user->socketfd > conn->pipefd[0]) ? user->socketfd : conn->pipefd[0]) + 1,
                                &rfdSet,NULL, &errfdSet, NULL);
            if (retVal >= 0) {
                char buff[10];
                int terminate = 0;

                if (FD_ISSET(conn->pipefd[0], &rfdSet)) {
                    char c;
                    read(conn->pipefd[0], &c, 1);
                    continue;
                }

                if (FD_ISSET(user->socketfd, &rfdSet)) {
                    unsigned int len = recv(user->socketfd, buff, 10, MSG_DONTWAIT | MSG_NOSIGNAL);
                    if (len > 0) {
                        buff[len] = '\0';
                        printf("[%ld][FINDER] Recived message: %s\n", pthread_self(), buff);
                        if (buff[0] == 'e') terminate = 1;
                    } else if (errno != EINTR) terminate = 1;
                }

                if (FD_ISSET(user->socketfd, &errfdSet) || terminate) {
                    printf("[%ld][FINDER] Try extract connection from waitlist\n", pthread_self());
                    pthread_mutex_lock(&room->mutex);
                    void *removeRet = extract(&room->waitlist, conn);
                    pthread_mutex_unlock(&room->mutex);

                    if (removeRet == NULL) { // Connection extract by other user
                        printf("[%ld][FINDER] Connection not found in waitlist closing connection\n", pthread_self());
                        if (isOpen(conn)) {
                            closeConnection(conn);
                            buff[0] = 'e';
                            buff[1] = '\n';
                            send(conn->user2->socketfd, buff, 2, MSG_NOSIGNAL);
                        } else closeConnection(conn);
                    } else {
                        printf("[%ld][FINDER] Connection removed\n", pthread_self());
                        closeConnection(conn); closeConnection(conn);
                    }
                    return NULL;
                }
            }
        }
        char c;
        while (read(conn->pipefd[0], &c, 1) != -1);

        if(room->time > 0){
            setConnectionTimeout(conn, room->time);
            startTimer(conn->timer);
        }
        return conn;
    } else {
        printf("[%ld][FINDER] Extract user from waitlist\n", pthread_self());
        Connection *conn = (Connection *) dequeue(&room->waitlist);
        connectUser(conn, user);
        pthread_mutex_unlock(&room->mutex);

        return conn;
    }
}