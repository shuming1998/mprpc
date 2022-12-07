aux_source_directory(. SRC)

add_library(mprpc ${SRC})
target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)