cmake_minimum_required(VERSION 3.14)
project(RandomChatServer C)

set(CMAKE_C_STANDARD 99)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(src)
enable_testing()
add_subdirectory(tests)

install(TARGETS RandomChatServer DESTINATION  /usr/local/bin)
