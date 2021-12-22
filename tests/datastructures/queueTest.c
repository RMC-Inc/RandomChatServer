#include <assert.h>
#include <datastructures/queue.h>
#include <string.h>

int main(){
    User u1;
    User u2;

    strcpy(u1.nickname, "Utente1");

    UserQueue q;
    newUserQueue(&q);
    assert(q.size == 0);

    enqueue(&q, &u1);
    assert(q.size == 1);
    assert(top(q) == &u1);

    assert(&u1 == dequeue(&q));
    assert(q.size == 0);
    assert(q.front == q.rear);
    assert(q.front == NULL);

    enqueue(&q, &u1);
    enqueue(&q, &u2);

    assert(q.size == 2);
    assert(top(q) == &u1);
    assert(dequeue(&q) == &u1);
    assert(top(q) == &u2);
    assert(q.size == 1);
    enqueue(&q, &u1);
    assert(top(q) == &u2);
    assert(dequeue(&q) == &u2);
    assert(dequeue(&q) == &u1);
    assert(q.size == 0);

    for (int i = 0; i < 10; ++i) {
        assert(q.size == i);
        enqueue(&q, &u1);
    }
    assert(q.size == 10);
    for (int i = 10; i > 0; --i) {
        assert(q.size == i);
        assert(dequeue(&q) == &u1);
    }

    assert(top(q) == NULL);
    assert(dequeue(&q) == NULL);
    assert(q.size == 0);

    enqueue(&q, &u1);
    assert(strcmp("Utente1", top(q)->nickname) == 0);
}