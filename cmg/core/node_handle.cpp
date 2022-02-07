/**
 * Author: Cyan
 * Date: Thu Dec  2 18:16:36 CST 2021
 */
#include "node_handle.hpp"

#include <cstdio>
#include <string>
#include <utility>

namespace cmg {

	Publisher::NodePublisher(std::shared_ptr<Sender> sender)
		: sender_(std::move(sender)) {}

	auto Publisher::publish(const Message &msg) -> bool {

		if (!this->sender_) {

			//CMG_DEBUG("Publisher publish: sender not set");
			return false;
		}

		return this->sender_->send(msg);
	}

	Subscriber::NodeSubscriber(std::shared_ptr<Receiver> receiver)
		: receiver_(std::move(receiver)) {}


	NodeHandle::NodeHandle(const std::string& proc_connect_to)
		: env_(Environment::Inst(proc_connect_to)), config_(Environment::Config()) {};

	void NodeHandle::shutdown() {

		cmg::Environment::Shutdown();
	}
}
