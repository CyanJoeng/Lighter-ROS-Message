/**
 * Author: Cyan
 * Date: Fri Dec  3 14:33:11 CST 2021
 */
#pragma once
#include <functional>
#include <memory>
#include <sstream>

#include "message.hpp"
#include "socket.hpp"

namespace cmg {

	class Receiver {

	protected:
		virtual ~Receiver() {};

	protected:
		using Msg_Ptr = std::shared_ptr<Message>;

	public:
		template <typename Msg>
		using Callback = std::function<void(const std::shared_ptr<Msg>&)>;

	public:
		virtual auto receive(std::stringstream &) -> bool = 0;
	};


	template <typename Msg>
	class SocketReceiver : public Receiver {

		Callback<Msg> callback_;

	public:
		SocketReceiver(std::shared_ptr<Socket> &socket, const Callback<Msg> &callback)
			: callback_(callback) {

			socket->startReceive([this](std::stringstream &ss) {

					this->receive(ss);
					});
		}

		~SocketReceiver() {}

		virtual auto receive(std::stringstream &ss) -> bool final {

			auto message = std::make_shared<Msg>();
			auto len = message->parse(ss);
			if (len != 0) {

				this->callback_(message);
				return true;
			}

			return false;
		}
	};

}
