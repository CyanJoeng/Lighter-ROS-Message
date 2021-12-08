cmake_minimum_required (VERSION 3.10)

project(CMG)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_subdirectory(src)

include_directories(
	src
	${CMAKE_CURRENT_BINARY_DIR}/src/message
	${CMAKE_CURRENT_BINARY_DIR}/src
	)
add_subdirectory(test)
