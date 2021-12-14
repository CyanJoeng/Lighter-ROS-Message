find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(pubsub_demo pubsub_demo.cpp)
target_link_libraries(pubsub_demo cmg)

add_executable(pubsub_point_cloud pubsub_point_cloud.cpp)
target_link_libraries(pubsub_point_cloud cmg)

add_executable(pubsub_image pubsub_image.cpp)
target_link_libraries(pubsub_image cmg
	${OpenCV_LIBS})
