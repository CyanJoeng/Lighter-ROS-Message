project(cmg)

set(nanomsg_DIR $ENV{PREFIX}/local/lib/cmake/nanomsg-1.1.5)
set(Boost_DIR $ENV{PREFIX}/lib/cmake/Boost-1.77.0)

find_package(nanomsg CONFIG REQUIRED)
find_package(Boost REQUIRED)

set(SRCS
	./cmg.cpp
	./socket.cpp
	./enviroment.cpp
	./node_handle.cpp
	)

add_library(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} nanomsg ${Boost_LIBRARIES})

