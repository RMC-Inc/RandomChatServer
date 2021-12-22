add_executable(RandomChatServer serverMain.c datastructures/queue.c datastructures/vector.c)
target_link_libraries(RandomChatServer PRIVATE Threads::Threads)
