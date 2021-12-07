/**
 * Author: Cyan
 * Date: Fri Dec  3 12:02:46 CST 2021
 */
#include "socket.hpp"

#include <map>
#include <sys/endian.h>

#include <cstdio>
#include <list>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>


namespace cmg {

	std::map<std::string, unsigned> URL::proc_ports = {
		{"server", BASE_PORT + 1},
		{"client", BASE_PORT + 2}
	};

	auto URL::Build(const std::string &address, unsigned port) -> const std::string {

		return address + ":" + std::to_string(port);
	}

	Socket::Socket(int protocol, unsigned port, const std::string &address, unsigned wait) {

		this->sid_ = nn_socket(AF_SP, protocol);
		if (this->sid_ < 0) {

			auto error_str = nn_strerror(nn_errno());
			fprintf(stderr, "socket open failed: (%s)\n", error_str);
			throw std::runtime_error(error_str);
		}

		this->url_ = URL::Build(address, port);
	};

	Socket::~Socket() {

		this->stopReceive();
	}

	auto Socket::Server(unsigned port, const std::string &address) -> std::shared_ptr<Socket> {

		printf("port %d\n", port);

		auto socket = std::shared_ptr<Socket>( new Socket(NN_PUB, port, address));
		auto ret = nn_bind(socket->sid_, socket->url_.c_str());
		if (ret < 0) {

			auto error_str = nn_strerror(nn_errno());
			fprintf(stderr, "socket bind failed: (bind to %s %s)\n", socket->url_.c_str(), error_str);
			throw std::runtime_error(error_str);
		}
		printf("Socket bind: %s\n", socket->url_.data());

		return socket;
	}

	auto Socket::send(const std::string &topic, const std::stringstream &ss) -> unsigned long {
		
		auto str = topic + '|' + ss.str();
		auto ret = nn_send(this->sid_, str.c_str(), str.length(), 0);
		if (ret < 0) {

			auto error_str = nn_strerror(nn_errno());
			fprintf(stderr, "socket send failed (%s)\n", error_str);
			throw  std::runtime_error(error_str);
		}
		printf("Socket send: %s\n", str.data());

		return ret - topic.length() - 1;
	}

	auto Socket::Client(const std::string &topic, unsigned port, const std::string &address, unsigned wait) -> std::shared_ptr<Socket> {

		auto socket = std::shared_ptr<Socket>(new Socket(NN_SUB, port, address, wait));
		auto ret = nn_connect(socket->sid_, socket->url_.c_str());
		if (ret < 0) {

			auto error_str = nn_strerror(nn_errno());
			fprintf(stderr, "socket connect failed (%s)\n", error_str);
			throw std::runtime_error(error_str);
		}
		printf("Socket connect: %s\n", socket->url_.c_str());

		ret = nn_setsockopt(socket->sid_, NN_SUB, NN_SUB_SUBSCRIBE, topic.c_str(), topic.length());
		if (ret < 0) {

			auto error_str = nn_strerror(nn_errno());
			fprintf(stderr, "socket set topic failed (%s)\n", error_str);
			throw std::runtime_error(error_str);
		}
		printf("Socket client set topic: %s\n", topic.data());

		return socket;
	}

	auto Socket::stopReceive() -> bool {

		this->exit_receive_ = true;
		if (this->reveiver_th_.joinable()) {

			this->reveiver_th_.join();
		}
		this->exit_receive_ = false;

		return true;
	}

	auto Socket::startReceive(const SsCallback &callback) -> bool {

		this->msg_callback_ = callback;

		auto receive_job = [this]() {

			while (!this->exit_receive_) {

				void * buf = nullptr;
				auto len = nn_recv(this->sid_, &buf, NN_MSG, 0);
				if (len < 0) {

					auto error_str = nn_strerror(nn_errno());
					fprintf(stderr, "socket receive failed (%s)\n", error_str);
					continue;
				}

				if (len == sizeof(u_int32_t)) {

					auto code = ntohl(*static_cast<uint32_t*>(buf));
					if (code == Socket::CODE_EXIT)
						break;
				}

				std::stringstream ss;
				ss.write(static_cast<const char *>(buf), len);
				this->msg_callback_(ss);

				nn_freemsg(buf);
			}
		};

		this->stopReceive();
		this->reveiver_th_ = std::thread(receive_job);

		return true;
	}
}
