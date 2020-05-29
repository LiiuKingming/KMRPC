# set(SRC_LIST userservice.cc ../user.pb.cc)
set(SRC_LIST friendservice.cc ../friend.pb.cc)

add_executable(provider ${SRC_LIST})
target_link_libraries(provider kmrpc protobuf)

# km: protobuf需要支持pthread,否则构建对象会崩溃
find_package(Threads)
target_link_libraries(provider ${CMAKE_THREAD_LIBS_INIT})