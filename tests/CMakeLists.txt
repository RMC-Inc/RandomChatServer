
include_directories(../src)

add_executable(QueueTest datastructures/queueTest.c ../src/datastructures/queue.c)


add_test(QUEUEtest QueueTest COMMAND $<TARGET_FILE:ttest>)


