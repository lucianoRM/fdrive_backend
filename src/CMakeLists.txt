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
include_directories("include")
include_directories("include/rocksdb")
if(CMAKE_COMPILER_IS_GNUCXX)
	#Compiler flags
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -g -std=c++11 -lrt -DMONGOOSE_ENABLE_THREADS")


	#Include directtarget_link_libraries(${TESTNAME} pthread libgtest)ories
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage -fPIC -O0")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fprofile-arcs -ftest-coverage -fPIC -O0")
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -fPIC -O0")

	include_directories("${CMAKE_CURRENT_LIST_DIR}/include")
	include_directories("${CMAKE_CURRENT_LIST_DIR}/include/server")
	include_directories("${CMAKE_CURRENT_LIST_DIR}/include/exceptions")
	include_directories("${CMAKE_CURRENT_LIST_DIR}/include/file")
	include_directories("${CMAKE_CURRENT_LIST_DIR}/include/folder")
	include_directories("${CMAKE_CURRENT_LIST_DIR}/include/manager")
	include_directories("${CMAKE_CURRENT_LIST_DIR}/include/mongoose")
	include_directories("${CMAKE_CURRENT_LIST_DIR}/include/user")
	include_directories("${CMAKE_CURRENT_LIST_DIR}/include/searchInformation")




endif()



#Libs and source files settings-------------------------------------------------------------------------------------------

#Rocksdb
add_library(rocksdb STATIC IMPORTED)
set_property(TARGET rocksdb PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/librocksdb.a)

#mongoose
set_source_files_properties( bin/mongoose/mongoose.c PROPERTIES LANGUAGE CXX)


#Object files
file(GLOB_RECURSE OSRCS "${CMAKE_CURRENT_LIST_DIR}/bin/*.[cph]*")
list(REMOVE_ITEM OSRCS ${CMAKE_CURRENT_LIST_DIR}/fdrive.cpp)
add_library(objectFiles OBJECT ${OSRCS})



#Exec file-------------------------------------------------------------------------------------------------------------

#Exec
add_executable(${EXECNAME} fdrive.cpp $<TARGET_OBJECTS:objectFiles>)

#Static library dependences
target_link_libraries(${EXECNAME} pthread rocksdb z crypto gcov)




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

add_executable(${TESTNAME} test/testMain/testMain.cpp $<TARGET_OBJECTS:individualTests> $<TARGET_OBJECTS:objectFiles>)
set_target_properties( ${TESTNAME}  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ./test )

# Standard linking to gtest stuff.
target_link_libraries(${TESTNAME} gtest gtest_main pthread rocksdb z crypto)










