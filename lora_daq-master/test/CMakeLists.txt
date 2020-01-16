cmake_minimum_required(VERSION 2.8.9)
project(lora_daq)
include_directories(include/)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
#for coma use: set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -g")
#set( CMAKE_VERBOSE_MAKEFILE on )
# -g for debugging later using gdb. remove afterwards.

#list(APPEND CMAKE_PREFIX_PATH /vol/optcoma/offline/ape/External/root/5.34.18/bin/)
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}$ENV{ROOTSYS}/etc/cmake/)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}/home/hpandya/root/root/etc/cmake/)
find_package(ROOT)
include_directories(${ROOT_INCLUDE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}/home/hpandya/boost/boost_1_68_0/)
find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})

add_library(functionslib src/functions_library.cxx)
add_library(buffer src/Buffer.cxx)
add_library(lora SHARED src/LORA_STATION_V1.cxx src/LORA_STATION.cxx)
add_library(operations src/OPERATIONS.cxx )
add_library(socket_calls src/Socket_Calls.cxx)
add_library(structs src/Structs.cxx)
add_executable(main src/main.cxx)
target_link_libraries(main operations lora socket_calls buffer structs functionslib ${ROOT_LIBRARIES} ${Boost_LIBRARIES})
