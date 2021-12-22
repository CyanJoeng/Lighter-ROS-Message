project(cmg)

if (NOT DEFINED nanomsg_DIR)
	set(nanomsg_DIR $ENV{PREFIX}/local/lib/cmake/nanomsg-1.1.5)
endif()

if(NOT DEFINED Boost_DIR)
	set(Boost_DIR $ENV{PREFIX}/lib/cmake/Boost-1.77.0)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

find_package(nanomsg REQUIRED)
list(APPEND EXT_LIBS nanomsg)

	find_package(Boost REQUIRED)
	include_directories(${Boost_INCLUDE_DIRS})

	list(APPEND MSG_SRCS messages/codex.cpp)
	list(APPEND EXT_LIBS ${Boost_LIBRARIES})

include_directories(
	${nanomsg_INCDIR}
	${OpenCV_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	)

list(APPEND CMG_SRCS
	cmg/cmg.cpp
	cmg/url.cpp
	cmg/socket.cpp
	cmg/enviroment.cpp
	cmg/node_handle.cpp
	)

add_subdirectory(protos)

list(APPEND MSG_SRCS
	messages/foo_bar.cpp
	messages/std_msgs/Header.hpp
	messages/geometry_msgs/point32.hpp
	messages/sensor_msgs/PointCloud.cpp
	messages/sensor_msgs/Image.cpp
	messages/nav_msgs/Odometry.cpp
	)

add_library(${PROJECT_NAME} SHARED
	${CMG_SRCS}
	${MSG_SRCS}
	)
target_link_libraries(${PROJECT_NAME} 
	${EXT_LIBS}

	proto_msg
	)

