/**
 * Author: CYan
 * Date: Thu Dec  2 16:18:22 CST 2021
 */
#include "cmg/cmg.hpp"
#include "messages/foo_bar.hpp"
#include "messages/message.hpp"
#include <thread>

using namespace cmg;

void cb0(const std::shared_ptr<example_msgs::FooBarMessage> &msg) {

	printf("client 0: %d,%s\n", msg->msg.id(), msg->msg.extra().data());

}

void cb1(const std::shared_ptr<example_msgs::FooBarMessage> &msg) {
	printf("client 1: %d,%s\n", msg->msg.id(), msg->msg.extra().data());
}


int main(int argc, char *argv[]) {

	const auto server_proc_name = "server";
	const auto client_proc_name = "client";

	std::string mode = argv[1];

	if (mode == "s") {

		cmg::init(argc, argv, server_proc_name);

		cmg::NodeHandle n("~");

		auto pub_foo = n.advertise<cmg::StrMessage>("foo", 1000);
		auto pub_bar = n.advertise<cmg::StrMessage>("bar", 1000);

		for (auto i = 0; i < 10;++i) {

			auto msg_foo = std::make_shared<example_msgs::FooBarMessage>(i, "foo");
			pub_foo.publish(msg_foo);

			std::this_thread::sleep_for(std::chrono::duration<double>(1.0));

			auto msg_bar = std::make_shared<example_msgs::FooBarMessage>(i, "bar");
			pub_bar.publish(msg_bar);

			std::this_thread::sleep_for(std::chrono::duration<double>(1.0));
		}

		cmg::spin();

	} else if (mode == "c") {

		std::string id = argv[2];

		cmg::init(argc, argv, client_proc_name);

		cmg::NodeHandle n("~");

		std::string proc_topic;
		if (id == "0") {

			proc_topic = "/server/foo";

			auto sub = n.subscribe(proc_topic, 1000, cb0);
			cmg::spin();

		} else if (id == "1") {

			proc_topic = "/server/bar";
			auto sub = n.subscribe(proc_topic, 1000, cb1);
			cmg::spin();
		}

	}

	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}
