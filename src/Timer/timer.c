#include "timer.h"
#include <unistd.h>
#include <stdlib.h>


void* timerThread(void* args){
    Timer* t = (Timer*) args;

    unsigned int remain = t->seconds;
    do{
        remain = sleep(remain);
    } while (remain != 0);

    t->isEnd = 1;

    if(t->fdnotify != 0){
        char c = 't';
        write(t->fdnotify, &c, 1);
    }
    pthread_exit(NULL);
}

Timer* newTimer(unsigned int seconds, int fd){
    Timer* t = malloc(sizeof(Timer));
    t->seconds = seconds;
    t->isEnd = 0;
    t->fdnotify = fd;
    t->tid = -1;
    return t;
}

void startTimer(Timer* t){
    pthread_create(&t->tid, NULL, timerThread, (void *) t);
}


void waitForTimer(Timer* t){
    pthread_join(t->tid, NULL);
}

void deleteTimer(Timer* t){
    if(!t->isEnd) pthread_cancel(t->tid);
    free(t);
}