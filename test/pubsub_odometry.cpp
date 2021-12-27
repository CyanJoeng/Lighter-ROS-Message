/**
 * Author: Cyan
 * Date: Wed Dec  8 16:33:46 CST 2021
 */
#include <cstdio>
#include <cstdlib>
#include <list>
#include <mutex>
#include <opencv2/core/types.hpp>
#include <string>
#include <thread>

#include <boost/program_options.hpp>

#include <opencv2/opencv.hpp>

#include "cmg/cmg.hpp"
#include "messages/nav_msgs/Odometry.hpp"

using namespace cmg;
namespace po = boost::program_options;

static std::string img_path;

auto draw_odo(const cmg::nav_msgs::OdometryConstPtr &odometry) -> cv::Mat {

	static auto bg_img = cv::imread(img_path, cv::IMREAD_ANYCOLOR);

	cv::Mat img = bg_img.clone();
	if (img.empty())
		img = cv::Mat::zeros(500, 1000, CV_8UC3);

	cv::putText(img, "stamp: " + std::to_string(odometry->header.stamp.toSec()), cv::Point(0, img.rows / 2), 0, 1., cv::Scalar {255, 255, 0}, 2);

	static cv::Point2d max_xy;

	auto &position = odometry->pose.position;
	max_xy.x = std::max(std::fabs(position.x), max_xy.x);
	max_xy.y = std::max(std::fabs(position.y), max_xy.y);

	static float scale = 1;

	auto odo_to_point = [w_2=img.cols/2, h_2=img.rows/2](auto x, auto y) -> cv::Point {

		float scale_x = max_xy.x * 10 > w_2 ? max_xy.x / w_2: 1;
		float scale_y = max_xy.y * 10 > h_2 ? max_xy.y / h_2: 1;

		scale = std::max(scale_x, scale_y);

		int off_x = (w_2 + (x * 10)) / scale + .5f;
		int off_y = (h_2 + (y * 10)) / scale + .5f;

		return {off_x, off_y};
	};

	static std::list<cmg::nav_msgs::OdometryConstPtr> odos;

	odos.push_back(odometry);

	cv::Point last_p(img.cols * .5f, img.rows * .5f);
	for (auto odo : odos) {

		auto pt = odo_to_point(odo->pose.position.x, odo->pose.position.y);
		cv::line(img, last_p, pt, cv::Scalar {255, 255, 0}, 2);

		//std::cout << last_p << pt << std::endl;
		last_p = pt;
	}

	return img;
}

auto create_odo(int i) -> cmg::nav_msgs::Odometry {

	return {};
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

static bool save_img = false;

auto cb(const cmg::nav_msgs::OdometryConstPtr &odometry) {

	auto &position = odometry->pose.position;
	printf("cb odometry stamp %f (%10.4f %10.4f %10.4f)\n", odometry->header.stamp.toSec(),
			position.x, position.y, position.z);

	auto img = draw_odo(odometry);
	{
		std::lock_guard<std::mutex> lock(img_mt);
		img.copyTo(show_img);
	}
}

auto args_parser(int argc, char *argv[]) -> po::variables_map {

	po::options_description desc("Socket connection test demo");
	desc.add_options()
		("help", "print this message")
		("mode", po::value<char>()->required(), "socker mode")
		("proc", po::value<std::string>()->default_value("server"), "server proc name")
		("topic", po::value<std::string>()->default_value("foo"), "topic name")
		("cfg", po::value<std::string>()->default_value(""), "config file")
		("bg_image", po::value<std::string>()->default_value(""), "background image of canvas");

	po::positional_options_description pos_desc;
	pos_desc.add("mode", 1);
		
	po::command_line_parser parser = po::command_line_parser(argc, argv).options(desc).positional(pos_desc);

	po::variables_map vm;
	po::store(parser.run(), vm);
	po::notify(vm);

	if (vm.count("help") || !vm.count("mode")) {

		std::cout << "Usage: " << argv[0] << " ";
		for (auto i = 0; i < pos_desc.max_total_count(); ++i)
			std::cout << pos_desc.name_for_position(i) << " ";
		std::cout << "[options]" << std::endl;
		
		std::cout << desc << std::endl;
		exit(0);
	}

	img_path = vm["bg_image"].as<std::string>();

	return vm;
}

int main(int argc, char *argv[]) {

	std::string server_proc_name = "server";
	std::string client_proc_name = "client";

	auto args = args_parser(argc, argv);

	char mode = args["mode"].as<char>();
	std::string proc = args["proc"].as<std::string>();
	std::string topic = args["topic"].as<std::string>();
	std::string cfg = args["cfg"].as<std::string>();

	const char *proc_args[] = {
		argv[0],
		cfg.data()
	};

	if (mode == 's') {

		std::string img_path = args["image"].as<std::string>();

		cmg::init(2, proc_args, server_proc_name.c_str());

		cmg::NodeHandle n("~");

		auto pub_odo = n.advertise<nav_msgs::Odometry>("odometry", 1000);

		for (auto i = 0; i >= 0; ++i) {

			auto msg_odo = create_odo(i);

			pub_odo.publish(msg_odo);
			printf("pub odometry\n");
			std::this_thread::sleep_for(std::chrono::duration<double>(.5));
		}

		cmg::spin();

	} else if (mode == 'c') {

		cmg::init(2, proc_args, client_proc_name.c_str());

		cmg::NodeHandle n("~");

		std::string proc_topic = "/" + proc + "/" + topic;

		work_loop = std::thread {
			[&]() {

				auto sub = n.subscribe(proc_topic, 1000, cb);
				cmg::spin();
			}
		};
		ui_refresh();
	}

	return 0;
}
