find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED
	program_options
	)

include_directories(
	${OpenCV_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	)

add_executable(pubsub_demo pubsub_demo.cpp)
target_link_libraries(pubsub_demo cmg ${Boost_LIBRARIES})

add_executable(pubsub_point_cloud pubsub_point_cloud.cpp)
target_link_libraries(pubsub_point_cloud cmg)

add_executable(pubsub_image pubsub_image.cpp)
target_link_libraries(pubsub_image cmg
	${OpenCV_LIBS} ${Boost_LIBRARIES})
