project(proto_msg)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS 
	foo_bar.proto
	std_msgs.proto
	geometry_msgs.proto
	sensor_msgs.proto
	)

add_library(${PROJECT_NAME}
	${PROTO_SRCS}
	)

target_link_libraries(${PROJECT_NAME} 
	${Protobuf_LIBRARIES}
	)
