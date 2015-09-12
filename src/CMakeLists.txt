cmake_minimum_required(VERSION 2.8)

set( EXECNAME fdrive)

project(${EXECNAME})

#C++ compiler
set(CMAKE_CXX_COMPILER g++)


#Compiler flags config

#WARNING: as default, it will compile .c files with gcc and .cpp files with g++.The following flags are just for g++ compiler, if needed .c files can be configured to be compiled with g++ compiler as so: set_source_files_properties(filename.c PROPERTIES LANGUAGE CXX ).

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "-W -Wall -I../.. -lpthread -g -Iinclude -std=c++11 -lz -lrt librocksdb.a")
endif()


#Rocksdb
add_library(rocksdb STATIC IMPORTED)
set_property(TARGET rocksdb PROPERTY IMPORTED_LOCATION ./librocksdb.a)

#mongoose
add_library(mongoose OBJECT mongoose.h mongoose.c)


#server
set_source_files_properties(server.cpp PROPERTIES LANGUAGE CXX)
add_library(server OBJECT server.h server.cpp)

#user
add_library(user OBJECT user.h user.cpp)



#Exec
add_executable(${EXECNAME} fdrive.cpp $<TARGET_OBJECTS:server> $<TARGET_OBJECTS:mongoose> $<TARGET_OBJECTS:user>)


#Static library dependences
target_link_libraries(${EXECNAME} pthread rocksdb)
