cmake_minimum_required(VERSION 2.8)

#Variable definition
set( EXECNAME fdrive)
set( TESTNAME allTests)
set( GTESTSOURCE ./include/googletest)


#Project name
project(${EXECNAME})




#Compiler setting-------------------------------------------------------------------------------------------

#Compiler flags config

#WARNING: as default, it will compile .c files with gcc and .cpp files with g++.The following flags are just for g++ compiler, if needed .c files can be configured to be compiled with g++ compiler as so: set_source_files_properties(filename.c PROPERTIES LANGUAGE CXX ).

if(CMAKE_COMPILER_IS_GNUCXX)
	#Compiler flags
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -g -std=c++11 -lrt -DMONGOOSE_ENABLE_THREADS")


	#Include directtarget_link_libraries(${TESTNAME} pthread libgtest)ories
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

	include_directories(include)

endif()



#Libs and source files settings-------------------------------------------------------------------------------------------

#Rocksdb
add_library(rocksdb STATIC IMPORTED)
set_property(TARGET rocksdb PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/librocksdb.a)

#mongoose
set_source_files_properties(mongoose.c PROPERTIES LANGUAGE CXX)
add_library(mongoose OBJECT mongoose.h mongoose.c)

#jsoncpp
add_library(jsoncpp OBJECT jsoncpp.cpp)

#server
add_library(server OBJECT server.h server.cpp )

#user
add_library(user OBJECT user.h user.cpp)

#requestHandler
add_library(requestHandler OBJECT requestHandler.h requestHandler.cpp request.h requestAddUser.h requestAddUser.cpp requestLogIn.h requestLogIn.cpp)


#Exec file-------------------------------------------------------------------------------------------------------------

#Exec
add_executable(${EXECNAME} fdrive.cpp $<TARGET_OBJECTS:requestHandler> $<TARGET_OBJECTS:server> $<TARGET_OBJECTS:mongoose> $<TARGET_OBJECTS:user> $<TARGET_OBJECTS:jsoncpp>)

#Static library dependences
#target_link_libraries(${EXECNAME} pthread rocksdb z bz2 snappy) # para la compu de DANI
target_link_libraries(${EXECNAME} pthread rocksdb z)




#Tests----------------------------------------------------------------------------------------------------------------------

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Iinclude/googletest/include")



#Individual test files
#This searchs for every file named *.cpp inside test folder. As the mainTest should not be compiled here, it should be somewhere else.
file(GLOB TESTSRCS "test/*.cpp")
add_library(individualTests OBJECT ${TESTSRCS})

# This adds another subdirectory, which has 'project(gtest)'.
add_subdirectory(${GTESTSOURCE})

enable_testing()
# Include the gtest library. gtest_SOURCE_DIR is available due to
# 'project(gtest)' above.
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(${TESTNAME} test/testMain/testMain.cpp $<TARGET_OBJECTS:individualTests>)
set_target_properties( ${TESTNAME}  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ./test )

# Standard linking to gtest stuff.
target_link_libraries(${TESTNAME} gtest gtest_main)










