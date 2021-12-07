cmake_minimum_required (VERSION 3.10)

project(CMG)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_subdirectory(cmg)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(test)
