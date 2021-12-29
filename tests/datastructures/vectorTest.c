#include <assert.h>
#include <datastructures/vector.h>
#include <stdio.h>

Room* generateRoom(unsigned int id){
    Room* new = malloc(sizeof (Room));
    new->id = id;
    return new;
}

int main(){
    RoomVector v;
    newVector(&v);
    printf("New Vector\n");

    int size = 0;
    assert(v.size == size);
    assert(v.size < v.realSize);

    printf("Insert element ");
    for (int i = 0; i < 200; i += 2) { // even number < 200
        Room* r = malloc(sizeof(Room));
        r->id = i;
        size++;
        add(&v, r);
    }

    printf("OK\n");

    assert(v.size == size);
    assert(v.size < v.realSize);

    printf("Check id");
    int j = 0;
    for (int i = 0; i < 200; i += 2) { // check id
        assert(v.rooms[j++]->id == i);
    }
    printf(" OK\n");

    printf("Add new elements ");
    add(&v, generateRoom(3));
    add(&v, generateRoom(9));
    add(&v, generateRoom(77));
    add(&v, generateRoom(153));
    add(&v, generateRoom(300));

    printf("OK\n");

    size += 5;
    assert(v.size == size);
    assert(v.size < v.realSize);


    printf("Check order ");
    for (int i = 0; i < v.size -1; ++i) {
        assert(v.rooms[i]->id < v.rooms[i+1]->id);
    }
    printf("OK\n");


    printf("Check getById ");
    assert(getbyId(&v, 0)->id == 0);
    assert(getbyId(&v, 300)->id == 300);
    assert(getbyId(&v, 120)->id == 120);
    assert(getbyId(&v, 3)->id == 3);
    assert(getbyId(&v, 9)->id == 9);
    assert(getbyId(&v, 153)->id == 153);
    assert(getbyId(&v, 2)->id == 2);
    assert(getbyId(&v, 100)->id == 100);

    assert(getbyId(&v, 1000) == NULL);
    assert(getbyId(&v, 5) == NULL);

    printf("OK\n");

    printf("Remove 3 obj \n");
    removeFrom(&v, 0);
    removeFrom(&v, 1);
    removeFrom(&v, v.size-1);
    size -= 3;
    printf("OK\n");


    printf("Check getById ");
    assert(v.size == size);
    assert(getbyId(&v, 0) == NULL);
    assert(getbyId(&v, 3) == NULL);
    assert(getbyId(&v, 300) == NULL);
    printf("OK\n");

    assert(removeById(&v, 153)->id == 153);
    assert(removeById(&v, 9)->id == 9);
    assert(removeById(&v, 500) == NULL);
    assert(removeById(&v, 5) == NULL);



    printf("Remove multiple rooms ");
    unsigned int realSiz = v.realSize;
    for (int i = 0; i < 90; ++i) {
        removeFrom(&v, 0);
    }
    assert(v.realSize < realSiz);
    printf(" OK\n");

    printf("Check only even ");
    for (int i = 0; i < v.size -1; ++i) {
        assert(v.rooms[i]->id < v.rooms[i+1]->id);
        assert(v.rooms[i]->id%2 == 0);
    }
    printf("OK\n");

    printf("Clear all ");
    while (v.size != 0) removeFrom(&v, 0);
    printf("OK\n");
    Room* r = malloc(sizeof(Room));
    r->id = 50;
    for (int i = 0; i < 400; ++i) {
        add(&v, r);
    }
    printf("Real size: %d\n", v.realSize);
    assert(v.realSize > 400);
    assert(getbyId(&v, 50) != NULL);
}
