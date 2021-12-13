project(cmg)

set(nanomsg_DIR $ENV{PREFIX}/local/lib/cmake/nanomsg-1.1.5)
set(Boost_DIR $ENV{PREFIX}/lib/cmake/Boost-1.77.0)
#set(OpenCV_DIR $ENV{HOME}/.OpenCV/sdk/native/jni)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

find_package(nanomsg REQUIRED)
find_package(Boost REQUIRED)
find_package(Protobuf 3 REQUIRED)
find_package(OpenCV 4 REQUIRED)

include_directories(
	${nanomsg_INCDIR}
	${Boost_INCLUDE_DIRS}
	${Protobuf_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	)

set(CMG_SRCS
	cmg/cmg.cpp
	cmg/socket.cpp
	cmg/enviroment.cpp
	cmg/node_handle.cpp
	)

add_subdirectory(protos)

set(MSG_SRCS
	messages/foo_bar.cpp
	messages/std_msgs/Header.hpp
	messages/geometry_msgs/point32.hpp
	messages/sensor_msgs/PointCloud.cpp
	messages/sensor_msgs/Image.cpp
	)

add_library(${PROJECT_NAME} SHARED
	${CMG_SRCS}
	${MSG_SRCS}
	)
target_link_libraries(${PROJECT_NAME} 
	nanomsg
	${Boost_LIBRARIES}
	${OpenCV_LIBS}

	proto_msg
	)

