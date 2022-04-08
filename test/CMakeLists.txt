option(CMG_WITH_VIZ "opencv with viz module" OFF)

find_package(Boost REQUIRED program_options)
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(
    ${EIGEN3_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    )

set(DEPS cmg cmg_msgs 
    ${Boost_LIBRARIES}
    ${OpenCV_LIBS}
    )

add_executable(pubsub_demo pubsub_demo.cpp)
target_link_libraries(pubsub_demo ${DEPS})

add_executable(pubsub_image pubsub_image.cpp)
target_link_libraries(pubsub_image ${DEPS})

if (CMG_WITH_VIZ)
    add_executable(pubsub_point_cloud pubsub_point_cloud.cpp)
    target_link_libraries(pubsub_point_cloud ${DEPS})

    add_executable(pubsub_odometry pubsub_odometry.cpp)
    target_link_libraries(pubsub_odometry ${DEPS})

    add_executable(pubsub_gather pubsub_gather.cpp)
    target_link_libraries(pubsub_gather ${DEPS})
endif()
