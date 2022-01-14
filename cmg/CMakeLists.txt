project(cmg)


list(APPEND DEP_LIBS
	nanomsg
	${Boost_LIBRARIES}
	z)

list(APPEND CMG_SRCS  # cmg logic
	cmg.cpp

	core/url.cpp
	core/socket.cpp
	core/enviroment.cpp
	core/node_handle.cpp

	utils/config.cpp

	message/codex.cpp
	)

add_library(${PROJECT_NAME} SHARED ${CMG_SRCS})
target_link_libraries(${PROJECT_NAME} ${DEP_LIBS})

