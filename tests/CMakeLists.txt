
include_directories(../src)

add_executable(RandomChatServerTest test.c ../src/Calculator.c)
enable_testing()
add_test(RCSTest RandomChatServerTest COMMAND $<TARGET_FILE:ttest>)

