cmake_minimum_required(VERSION 3.15)
project(mprpc)

# set(CMAKE_CXX_FLAGS "$ {CMAKE_CXX_FLAGS} -fPIC")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/example)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/moduo)

link_directories(${PROJECT_SOURCE_DIR}/lib)
link_directories(${PROJECT_SOURCE_DIR}/thirdparty)

add_subdirectory(src)
add_subdirectory(example)

