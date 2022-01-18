project(cmg_msgs)
find_package(Protobuf 3 REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS 
	protos/example.proto
	protos/geometry_msgs.proto
	protos/nav_msgs.proto
	protos/sensor_msgs.proto
	protos/std_msgs.proto
	)
include_directories(${PROJECT_BINARY_DIR})

add_library(${PROJECT_NAME} SHARED
	foo_bar.cpp
	StampedFloat64Array.cpp
	nav_msgs/Odometry.cpp
	sensor_msgs/Image.cpp
	sensor_msgs/Imu.cpp
	sensor_msgs/NavSatFix.cpp
	sensor_msgs/PointCloud.cpp
	${PROTO_SRCS})

if (${CMAKE_SYSTEM_NAME} MATCHES "Android")
	target_link_libraries(${PROJECT_NAME}
		${Protobuf_LIBRARIES} log cmg
		)
else()
	target_link_libraries(${PROJECT_NAME}
		${Protobuf_LIBRARIES} cmg
		)
endif()
