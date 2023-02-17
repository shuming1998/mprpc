set(SRC callfriendservice.cpp ../friend.pb.cc)

add_executable(consumer ${SRC})
target_link_libraries(consumer mprpc protobuf)
