project(cmg)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

find_package(nanomsg REQUIRED)
list(APPEND EXT_LIBS nanomsg)

find_package(Boost 1.75.0 COMPONENTS iostreams
	HINTS ${Boost_DIR})

if (NOT Boost_FOUND)
	message("set Boost manually")
	message("Boost include dir ${Boost_INCLUDE_DIR}")
	message("Boost lib dir ${Boost_LIBRARY_DIR}")

	set(Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIR})
	file(GLOB Boost_LIBRARIES LIST_DIRECTORIES false
		${Boost_LIBRARY_DIR}/libboost_*.a)
endif()

list(APPEND MSG_SRCS messages/codex.cpp)
list(APPEND EXT_LIBS ${Boost_LIBRARIES} z)

include_directories(
	${nanomsg_INCDIR}
	${Boost_INCLUDE_DIRS}
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

