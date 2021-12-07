
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS foo_bar.proto)

add_library(proto_msg
	${PROTO_SRCS}
	)
