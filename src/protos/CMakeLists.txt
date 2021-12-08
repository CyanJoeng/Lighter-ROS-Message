
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS 
	foo_bar.proto
	std_msgs.proto
	geometry_msgs.proto
	sensor_msgs.proto
	)

add_library(proto_msg
	${PROTO_SRCS}
	)
