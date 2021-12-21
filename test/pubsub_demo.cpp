/**
 * Author: CYan
 * Date: Thu Dec  2 16:18:22 CST 2021
 */
#include <thread>

#include <boost/program_options.hpp>

#include "cmg/cmg.hpp"
#include "messages/foo_bar.hpp"
#include "messages/message.hpp"

using namespace cmg;

namespace po = boost::program_options;

void cb(const std::shared_ptr<example_msgs::FooBarMessage> &msg) {

	printf("client 0: %d %f %s\n", msg->id, msg->timestamp, msg->extra.data());
}


auto args_parser(int argc, char *argv[]) -> po::variables_map {

	po::options_description desc("Socket connection test demo");
	desc.add_options()
		("help", "print this message")
		("mode", po::value<char>(), "socker mode")
		("proc", po::value<std::string>()->default_value("server"), "server proc name")
		("topic", po::value<std::string>()->default_value("foo"), "topic name")
		("cfg", po::value<std::string>()->default_value(""), "config file");

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

	return vm;
}


int main(int argc, char *argv[]) {

	const auto server_proc_name = "server";
	const auto client_proc_name = "client";

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

		cmg::init(2, proc_args, server_proc_name);

		cmg::NodeHandle n("~");

		auto pub_foo = n.advertise<cmg::StrMessage>("foo", 1000);
		auto pub_bar = n.advertise<cmg::StrMessage>("bar", 1000);

		for (auto i = 0; i < 1000;++i) {

			auto msg_foo = std::make_shared<example_msgs::FooBarMessage>(i, (rand() % (int)1e6) * 1e-3, "foo");
			pub_foo.publish(msg_foo);

			std::this_thread::sleep_for(std::chrono::duration<double>(1.0));

			auto msg_bar = std::make_shared<example_msgs::FooBarMessage>(i, (rand() % (int)1e6) * 1e-3, "bar");
			pub_bar.publish(msg_bar);

			std::this_thread::sleep_for(std::chrono::duration<double>(.5));
		}

		cmg::spin();

	} else if (mode == 'c') {

		cmg::init(2, proc_args, client_proc_name);

		cmg::NodeHandle n("~");

		std::stringstream ss;
		ss << "/" << proc << "/" << topic;

		auto sub = n.subscribe(ss.str(), 1000, cb);
		cmg::spin();
	}

	return 0;
}
