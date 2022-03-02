/**
 * Author: Cyan
 * Date: Thu Dec  2 16:16:47 CST 2021
 */
#pragma once

#include <cassert>
#include <cstdio>
#include <type_traits>

#include <memory>

#include "cmg/message/message.hpp"
#include "cmg/core/socket.hpp"
#include "cmg/utils/log.hpp"

namespace cmg {

	class Sender {

	protected:
		virtual ~Sender() {}

	public:
		virtual auto send(const Message &msg) -> bool = 0;
	};


	class SocketSender : public Sender {

		std::shared_ptr<Socket> socket_;

		std::string topic_;

		unsigned wait_;

	public:
		SocketSender(std::shared_ptr<Socket> &socket, const std::string &topic, unsigned wait)
			: socket_(socket), topic_(topic), wait_(wait) {};

		~SocketSender() {}

		virtual auto send(const Message &msg) -> bool final {

			std::stringstream ss;
			auto length = msg.serialize(ss);

			if (ss.str().length() != length) {

				CMG_WARN("stream len(%lu) != serialize len(%lu)", ss.str().length(), length);
				return false;
			}

			try {

				auto send_len = this->socket_->send(this->topic_, ss);
				//CMG_DEBUG("SocketSender send msg_cipher_len %zu", send_len);

			} catch (const std::runtime_error &e) {

				CMG_WARN("SocketSender send error: %s", e.what());
			}

			return true;
		}
	};
}
