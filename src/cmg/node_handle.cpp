/**
 * Author: Cyan
 * Date: Thu Dec  2 18:16:36 CST 2021
 */
#include "node_handle.hpp"

#include <cstdio>
#include <string>

namespace cmg {

	Publisher::NodePublisher(const std::shared_ptr<Sender> &sender)
		: sender_(sender) {}

	auto Publisher::publish(const std::shared_ptr<Message> &msg) -> bool {

		if (!this->sender_) {

			printf("Publisher publish: sender not set\n");
			return false;
		}

		return this->sender_->send(msg);
	}

	Subscriber::NodeSubscriber(const std::shared_ptr<Receiver> &receiver)
		: receiver_(receiver) {}


	NodeHandle::NodeHandle(const std::string &proc_connect)
		: proc_name_(proc_connect) {};

	void NodeHandle::shutdown() {

		Environment::Inst(this->proc_name_).Shutdown();
	}
}
