project(proto_msg)

find_package(Protobuf 3 REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS 
	foo_bar.proto
	std_msgs.proto
	geometry_msgs.proto
	sensor_msgs.proto
	)

add_library(${PROJECT_NAME} SHARED
	${PROTO_SRCS}
	)

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Android")
	target_link_libraries(${PROJECT_NAME}
		${Protobuf_LIBRARIES} log
		)
else()
	target_link_libraries(${PROJECT_NAME}
		${Protobuf_LIBRARIES}
		)
endif()
