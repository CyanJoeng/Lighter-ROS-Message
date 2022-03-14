cmake_minimum_required (VERSION 3.10)
project(CMG)

option(CMG_BUILD_APPS "build CMG test apps" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

find_package(nanomsg REQUIRED)
find_package(Boost 1.75.0 COMPONENTS iostreams filesystem
	HINTS ${Boost_DIR} QUIET)

if (NOT Boost_FOUND)
	message("set Boost manually")
	message("Boost include dir ${Boost_INCLUDE_DIR}")
	message("Boost lib dir ${Boost_LIBRARY_DIR}")

	set(Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIR})
	file(GLOB Boost_LIBRARIES LIST_DIRECTORIES false
		${Boost_LIBRARY_DIR}/libboost_*.a)
endif()

include_directories(
	${nanomsg_INCDIR}
	${Boost_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}
	)

add_subdirectory(cmg)
add_subdirectory(messages)

set(CMG_LIBS cmg cmg_msgs CACHE STRING "cmg librarys")
set(CMG_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "cmg includes")

if (${CMG_BUILD_APPS})
	add_subdirectory(test)
endif()
