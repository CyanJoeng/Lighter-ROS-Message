find_package(Boost REQUIRED program_options)
find_package(OpenCV REQUIRED)

add_executable(pubsub_demo pubsub_demo.cpp)
target_link_libraries(pubsub_demo cmg cmg_msgs
	${Boost_LIBRARIES})

add_executable(pubsub_point_cloud pubsub_point_cloud.cpp)
target_link_libraries(pubsub_point_cloud cmg cmg_msgs)

add_executable(pubsub_image pubsub_image.cpp)
target_link_libraries(pubsub_image cmg cmg_msgs
	${OpenCV_LIBS} ${Boost_LIBRARIES})

add_executable(pubsub_odometry pubsub_odometry.cpp)
target_link_libraries(pubsub_odometry cmg cmg_msgs
	${OpenCV_LIBS} ${Boost_LIBRARIES})
