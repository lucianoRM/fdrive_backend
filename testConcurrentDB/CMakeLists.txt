cmake_minimum_required(VERSION 2.8)
project(testConcurrentDB)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(rocksdb STATIC IMPORTED)
set_property(TARGET rocksdb PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/librocksdb.a)


set(SOURCE_FILES main.cpp)
add_executable(testConcurrentDB ${SOURCE_FILES})
target_link_libraries(testConcurrentDB pthread rocksdb crypto z )