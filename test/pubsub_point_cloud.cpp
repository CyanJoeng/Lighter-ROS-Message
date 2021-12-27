/**
 * Author: Cyan
 * Date: Tue Dec  7 23:30:10 CST 2021
 */

#include "cmg/cmg.hpp"
#include "messages/sensor_msgs/PointCloud.hpp"
#include "protos/geometry_msgs.pb.h"
#include "protos/sensor_msgs.pb.h"
#include "protos/std_msgs.pb.h"
#include <cstdio>
#include <cstdlib>
#include <memory>


using namespace cmg;

auto create_point_cloud() -> sensor_msgs::PointCloud {

	sensor_msgs::PointCloud feature_points;

	sensor_msgs::ChannelFloat32 id_of_point;
	sensor_msgs::ChannelFloat32 u_of_point;
	sensor_msgs::ChannelFloat32 v_of_point;
	sensor_msgs::ChannelFloat32 velocity_x_of_point;
	sensor_msgs::ChannelFloat32 velocity_y_of_point;

	//feature_points.header;  // img_msg->header;
	feature_points.header.frame_id = "world";
	feature_points.header.stamp.time_ = (rand() % 1000 * 1e-3);

	for (auto i = rand() % 1000; i < 1000; ++i) {

		geometry_msgs::Point32 p;
		p.x = i * 0.01;
		p.y = i * 0.02;
		p.z = 1;

		feature_points.points.push_back(p);

		id_of_point.values.push_back(i);
		u_of_point.values.push_back(i * 10);
		v_of_point.values.push_back(i * 11);
		velocity_x_of_point.values.push_back(i * 1.0);
		velocity_y_of_point.values.push_back(i * 1.1);
	}

	feature_points.channels.push_back(id_of_point);
	feature_points.channels.push_back(u_of_point);
	feature_points.channels.push_back(v_of_point);
	feature_points.channels.push_back(velocity_x_of_point);
	feature_points.channels.push_back(velocity_y_of_point);

	return feature_points;
}

auto cb(const std::shared_ptr<const sensor_msgs::PointCloud> &point_cloud) {

	printf("cb point_cloud count %zu stamp %f\n", point_cloud->points.size(), point_cloud->header.stamp.toSec());
}

int main(int argc, char *argv[]) {

	const auto server_proc_name = "server";
	const auto client_proc_name = "client";

	std::string mode = argv[1];

	if (mode == "s") {

		cmg::init(argc, argv, server_proc_name);

		cmg::NodeHandle n("~");

		auto pub_point_cloud = n.advertise<sensor_msgs::PointCloud>("foo", 1000);

		for (auto i = 0; i < 10;++i) {

			auto msg_point_cloud = create_point_cloud();
			pub_point_cloud.publish(msg_point_cloud);

			std::this_thread::sleep_for(std::chrono::duration<double>(1.0));
		}

		cmg::spin();

	} else if (mode == "c") {

		cmg::init(argc, argv, client_proc_name);

		cmg::NodeHandle n("~");

		std::string proc_topic = "/server/foo";

		auto sub = n.subscribe(proc_topic, 1000, cb);
		cmg::spin();
	}

	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}
