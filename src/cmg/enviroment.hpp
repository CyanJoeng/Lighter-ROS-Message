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

	public:
		static auto Inst(const std::string &proc_name = "") -> Environment&;

		static void Spin();

	private:
		static std::map<std::string, Environment> insts;

	public:
		~Environment();

	private:
		Environment(const std::string &proc_name);

	private:
		const URL& url_;

		std::shared_ptr<Socket> server_;

		std::map<std::string, std::shared_ptr<Socket>> clients_;

	public:
		template <typename Msg>
		auto sender(const std::string &topic, unsigned wait) -> std::shared_ptr<Sender> {

			if (!this->server_) {

				this->server_ = Socket::Server(this->url_);
			}

			auto sender = std::make_shared<SocketSender>(this->server_, topic, wait);

			return sender;
		}

		template <typename Msg>
		auto receiver(const std::string &topic, unsigned wait, const Receiver::Callback<Msg> &callback) -> std::shared_ptr<Receiver> {

			auto client = Socket::Client(this->url_);
			this->clients_.emplace(topic, client);

			auto receiver = std::make_shared<SocketReceiver<Msg>>(client, topic, callback);
			return receiver;
		}
	};
}
