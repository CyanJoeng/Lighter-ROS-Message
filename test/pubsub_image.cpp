/**
 * Author: Cyan
 * Date: Wed Dec  8 16:33:46 CST 2021
 */
#include <cstdio>
#include <opencv2/imgcodecs.hpp>
#include <protos/foo_bar.pb.h>

#include "cmg/cmg.hpp"
#include "messages/sensor_msgs/Image.hpp"

using namespace cmg;

auto create_image(const std::string &img_path) -> sensor_msgs::ImagePtr {

	sensor_msgs::ImagePtr image(new sensor_msgs::Image);

	image->header;  // img_msg->header;
	image->header.frame_id = "world";
	image->header.stamp.time_ = (rand() % 1000 * 1e-3);

	image->image = cv::imread(img_path, cv::IMREAD_ANYCOLOR);

	return image;
}

auto cb(const std::shared_ptr<sensor_msgs::Image> &image) {

	auto image_ = &image->image;
	printf("cb image size (%d, %d) stamp %f\n", image_->rows, image_->cols, image->header.stamp.toSec());

	cv::imwrite("image.png", *image_);
	printf("write image to image.png\n");
}

int main(int argc, char *argv[]) {

	std::string server_proc_name = "server";
	std::string client_proc_name = "client";

	std::string mode = argv[1];

	if (mode == "s") {

		std::string img_path = argv[2];

		cmg::init(argc, argv, server_proc_name.c_str());

		cmg::NodeHandle n("~");

		auto pub_point_cloud = n.advertise<sensor_msgs::Image>("foo", 1000);

		for (auto i = 0; i < 10;++i) {

			auto msg_point_cloud = create_image(img_path);
			pub_point_cloud.publish(msg_point_cloud);

			std::this_thread::sleep_for(std::chrono::duration<double>(1.0));
		}

		cmg::spin();

	} else if (mode == "c") {

		std::string topic = "foo";

		if (argc > 2)
			client_proc_name = argv[2];
		if (argc > 3)
			topic = argv[3];

		cmg::init(argc, argv, client_proc_name.c_str());

		cmg::NodeHandle n("~");

		std::string proc_topic = "/" + client_proc_name + "/" + topic;

		auto sub = n.subscribe(proc_topic, 1000, cb);
		cmg::spin();
	}

	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}
