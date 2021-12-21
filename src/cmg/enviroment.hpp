/**
 * Author: Cyan
 * Date: Fri Dec  3 11:59:11 CST 2021
 */
#pragma once
#include <memory>
#include <string>
#include <map>

#include "sender.hpp"
#include "receiver.hpp"
#include "socket.hpp"
#include "url.hpp"

namespace cmg {

	/**
	 * socket container
	 * if in cross process/machine, one server to multipel clients
	 *		server <--> | client
	 *					| client
	 * if in one process, servers charged by static map
	 */
	class Environment {
	public:
		static constexpr auto MULTI_PROC_NAME = "~";

		using TopicType = std::string;

		using ProcType = std::string;

	public:
		static auto Inst(const std::string &proc_name = "") -> Environment&;

		static void Spin();

	private:
		static std::map<ProcType, Environment> insts;

	public:
		~Environment();

	private:
		Environment(const std::string &proc_name);

	private:
		const std::string proc_name_;

		std::map<TopicType, std::shared_ptr<Socket>> servers_;

		std::map<TopicType, std::shared_ptr<Socket>> clients_;

	public:
		template <typename Msg>
		auto sender(const std::string &topic, unsigned wait) -> std::shared_ptr<Sender> {

			if (this->servers_.end() == this->servers_.find(topic)) {

				auto server = Socket::Server(URL::Inst(this->proc_name_, topic));
				this->servers_.emplace(topic, server);
			}

			auto sender = std::make_shared<SocketSender>(this->servers_[topic], topic, wait);

			return sender;
		}

		template <typename Msg>
		auto receiver(const std::string &topic, unsigned wait, const Receiver::Callback<Msg> &callback) -> std::shared_ptr<Receiver> {

			if (this->clients_.end() == this->clients_.find(topic)) {

				auto client = Socket::Client(URL::Inst(this->proc_name_, topic));
				this->clients_.emplace(topic, client);
			}

			auto receiver = std::make_shared<SocketReceiver<Msg>>(this->clients_[topic], topic, callback);
			return receiver;
		}
	};
}
