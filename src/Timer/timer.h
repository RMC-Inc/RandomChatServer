#ifndef RANDOMCHATSERVER_TIMER_H
#define RANDOMCHATSERVER_TIMER_H
#include <pthread.h>


typedef struct {
    unsigned int seconds;
    int fdnotify;

    int isEnd;
    pthread_t tid;
} Timer;

Timer* newTimer(unsigned int seconds, int fd); // fd 0 -> no notify
void startTimer(Timer* t);


void waitForTimer(Timer* t); // blocks until terminate

void deleteTimer(Timer* t);



#endif //RANDOMCHATSERVER_TIMER_H
