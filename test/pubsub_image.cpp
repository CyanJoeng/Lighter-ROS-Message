/**
 * Author: Cyan
 * Date: Wed Dec  8 16:33:46 CST 2021
 */
#include <cstdio>
#include <mutex>
#include <thread>

#include <boost/program_options.hpp>

#include <opencv2/opencv.hpp>

#include "cmg/cmg.hpp"
#include "messages/sensor_msgs/Image.hpp"

using namespace cmg;
namespace po = boost::program_options;


auto create_image(const std::string &img_path) -> sensor_msgs::ImagePtr {

	sensor_msgs::ImagePtr image(new sensor_msgs::Image);

	//image->header;  // img_msg->header;
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

static bool save_img = false;

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

auto args_parser(int argc, char *argv[]) -> po::variables_map {

	po::options_description desc("Socket connection test demo");
	desc.add_options()
		("help", "print this message")
		("mode", po::value<char>()->required(), "socker mode")
		("proc", po::value<std::string>()->default_value("server"), "server proc name")
		("topic", po::value<std::string>()->default_value("foo"), "topic name")
		("cfg", po::value<std::string>()->default_value(""), "config file")
		("save_img", po::value<bool>(), "image be published")
		("image", po::value<std::string>(), "store received image");

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

	if (vm.count("save_img"))
		save_img = true;

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

	} else if (mode == 'c') {

		std::string topic = "foo";

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
