# set(SRC_LIST calluserservice.cc ../user.pb.cc)
set(SRC_LIST callfriendservice.cc ../friend.pb.cc)

add_executable(consumer ${SRC_LIST})
target_link_libraries(consumer kmrpc protobuf)

# km: protobuf需要支持pthread,否则构建对象会崩溃
find_package(Threads)
target_link_libraries(consumer ${CMAKE_THREAD_LIBS_INIT})