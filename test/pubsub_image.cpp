/**
 * Author: Cyan
 * Date: Wed Dec  8 16:33:46 CST 2021
 */
#include <cstdio>
#include <mutex>
#include <thread>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "cmg/cmg.hpp"
#include "protos/foo_bar.pb.h"
#include "messages/sensor_msgs/Image.hpp"

using namespace cmg;

bool save_img = true;

auto create_image(const std::string &img_path) -> sensor_msgs::ImagePtr {

	sensor_msgs::ImagePtr image(new sensor_msgs::Image);

	image->header;  // img_msg->header;
	image->header.frame_id = "world";
	image->header.stamp.time_ = (rand() % 1000 * 1e-3);

	static auto ori_img = cv::imread(img_path, cv::IMREAD_ANYCOLOR);

	cv::Mat img = ori_img.clone();

	printf("cv point %d\n", img.rows/2);
	cv::putText(img, "stamp: " + std::to_string(image->header.stamp.toSec()), cv::Point(0, img.rows / 2), 0, 1., cv::Scalar {255, 255, 0}, 2);

	img(cv::Rect(0, 0, 30, 30)) = cv::Mat::ones(30, 30, CV_8UC3) * 128;

	cv::Mat data;
	img.copyTo(data);
	image->setData(img.rows, img.cols, img.channels(), (char*)data.data);

	return image;
}

static cv::Mat show_img;
static std::mutex img_mt;
static std::thread work_loop;

void ui_refresh() {

	while (true) {

		if (!show_img.empty()) {

			{
				std::lock_guard<std::mutex> lock(img_mt);

				cv::imshow("callback", show_img);
			}
			cv::waitKey(10);
		}

		std::this_thread::sleep_for(std::chrono::duration<double>(.1));
	}
}

auto cb(const std::shared_ptr<sensor_msgs::Image> &image) {

	printf("cb image size (%d, %d) stamp %f\n", image->rows, image->cols, image->header.stamp.toSec());

	{

		std::lock_guard<std::mutex> lock(img_mt);
		cv::Mat(image->rows, image->cols, CV_8UC(image->channels), (void*)image->data.data()).copyTo(show_img);
	}
	if (save_img) {

		char path[64];
		sprintf(path, "out/image_%.5f.png", image->header.stamp.toSec());
		cv::imwrite(path, show_img);
		printf("write image to image.png\n");
	}
}

int main(int argc, char *argv[]) {

	std::string server_proc_name = "server";
	std::string client_proc_name = "client";

	std::string mode = argv[1];

	if (mode == "s") {

		std::string img_path = argv[2];

		cmg::init(argc - 2, argv + 2, server_proc_name.c_str());

		cmg::NodeHandle n("~");

		auto pub_image_foo = n.advertise<sensor_msgs::Image>("foo", 1000);

		auto pub_image_bar = n.advertise<sensor_msgs::Image>("bar", 1000);

		for (auto i = 0; i >= 0; ++i) {

			auto msg_image = create_image(img_path);

			pub_image_foo.publish(msg_image);
			printf("pub foo\n");
			std::this_thread::sleep_for(std::chrono::duration<double>(.5));

			pub_image_bar.publish(msg_image);
			printf("pub bar\n");
			std::this_thread::sleep_for(std::chrono::duration<double>(.5));
		}

		cmg::spin();

	} else if (mode == "c") {

		std::string topic = "foo";

		if (argc > 2)
			server_proc_name = argv[2];
		if (argc > 3)
			topic = argv[3];

		cmg::init(argc - 3, argv + 3, client_proc_name.c_str());

		cmg::NodeHandle n("~");

		std::string proc_topic = "/" + server_proc_name + "/" + topic;

		work_loop = std::thread {
			[&]() {

				auto sub = n.subscribe(proc_topic, 1000, cb);
				cmg::spin();
			}
		};
		ui_refresh();
	}

	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}
