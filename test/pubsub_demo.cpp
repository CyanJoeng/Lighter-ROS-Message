/**
 * Author: CYan
 * Date: Thu Dec  2 16:18:22 CST 2021
 */
#include "cmg/cmg.hpp"
#include "cmg/message.hpp"
#include <thread>


int main(int argc, char *argv[]) {

	const auto server_proc_name = "server";
	const auto client_proc_name = "client";

	std::string mode = argv[1];

	if (mode == "s") {

		cmg::init(argc, argv, server_proc_name);

		cmg::NodeHandle n("~");

		auto pub_fol = n.advertise<cmg::StrMessage>("foo", 1000);
		auto pub_bar = n.advertise<cmg::StrMessage>("bar", 1000);

		for (auto i = 0; i < 10;++i) {

			auto msg_fol = std::make_shared<cmg::StrMessage>("foo:" + std::to_string(i));
			pub_fol.publish(msg_fol);

			std::this_thread::sleep_for(std::chrono::duration<double>(1.0));

			auto msg_bar = std::make_shared<cmg::StrMessage>("bar:" + std::to_string(i));
			pub_bar.publish(msg_bar);

			std::this_thread::sleep_for(std::chrono::duration<double>(1.0));
		}

		cmg::spin();

	} else if (mode == "c") {

		std::string id = argv[2];

		cmg::init(argc, argv, client_proc_name);

		cmg::NodeHandle n("~");

		std::function<void (std::shared_ptr<cmg::StrMessage>)> cb0 = [](const std::shared_ptr<cmg::StrMessage> &msg) {

			printf("client 0: %s\n", msg->data_.data());
		};

		std::function<void (std::shared_ptr<cmg::StrMessage>)> cb1 = [](const std::shared_ptr<cmg::StrMessage> &msg) {

			printf("client 1: %s\n", msg->data_.data());
		};

		std::function<void (std::shared_ptr<cmg::StrMessage>)> cb;
		std::string proc_topic;
		if (id == "0") {

			cb = cb0;
			proc_topic = "/server/foo";

		} else if (id == "1") {

			cb = cb1;
			proc_topic = "/server/bar";
		}

		auto sub = n.subscribe<cmg::StrMessage>(proc_topic, 1000, cb);

		cmg::spin();
	}


	return 0;
}
