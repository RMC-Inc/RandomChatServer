
include_directories(../src)

add_executable(QueueTest datastructures/queueTest.c ../src/datastructures/queue.c)
add_executable(VectorTest datastructures/vectorTest.c ../src/datastructures/vector.c)


add_test(QUEUEtest QueueTest COMMAND $<TARGET_FILE:ttest>)
add_test(VECTORtest VectorTest COMMAND $<TARGET_FILE:ttest>)


