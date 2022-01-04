project(cmg)

if (NOT DEFINED nanomsg_DIR)
	set(nanomsg_DIR $ENV{PREFIX}/local/lib/cmake/nanomsg-1.1.5)
endif()

if(NOT DEFINED Boost_DIR)
	set(Boost_DIR $ENV{PREFIX}/lib/cmake/Boost-1.77.0)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

find_package(OpenCV 4 REQUIRED)
find_package(nanomsg REQUIRED)
list(APPEND EXT_LIBS nanomsg ${OpenCV_LIBS})

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

list(APPEND CMG_SRCS  # utils
	utils/config.cpp
	)

list(APPEND CMG_SRCS  # cmg logic
	cmg/cmg.cpp
	cmg/url.cpp
	cmg/socket.cpp
	cmg/enviroment.cpp
	cmg/node_handle.cpp
	)

add_subdirectory(protos)

list(APPEND CMG_SRCS  # messages 
	messages/codex.cpp
	messages/foo_bar.cpp
	messages/sensor_msgs/PointCloud.cpp
	messages/sensor_msgs/Image.cpp
	messages/sensor_msgs/Imu.cpp
	messages/sensor_msgs/NavSatFix.cpp
	messages/nav_msgs/Odometry.cpp
	messages/StampedFloat64Array.cpp
	)

add_library(${PROJECT_NAME} SHARED ${CMG_SRCS})
target_link_libraries(${PROJECT_NAME} 
	${EXT_LIBS}

	cmg_proto_msg
	)

