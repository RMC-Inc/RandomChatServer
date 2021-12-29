add_executable(RandomChatServer serverMain.c datastructures/queue.c datastructures/vector.c server.c server.h datastructures/entity/room.c Finder/finder.c Finder/finder.h)
target_link_libraries(RandomChatServer PRIVATE Threads::Threads)
