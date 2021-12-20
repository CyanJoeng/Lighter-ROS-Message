/**
 * Author: Cyan
 * Date: Fri Dec  3 12:02:46 CST 2021
 */
#include "socket.hpp"

#include <arpa/inet.h>

#include <cstdio>
#include <stdexcept>
#include <string>
#include <thread>

#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>


namespace cmg {

	Socket::Socket(int protocol, const URL &url, unsigned wait)
		: url_(url) {

		this->sid_ = nn_socket(AF_SP, protocol);
		if (this->sid_ < 0) {

			auto error_str = nn_strerror(nn_errno());
			fprintf(stderr, "socket open failed: (%s)\n", error_str);
			throw std::runtime_error(error_str);
		}
	};

	Socket::~Socket() {

		this->stopReceive();
	}

	auto Socket::Server(const URL &url) -> std::shared_ptr<Socket> {

		auto socket = std::shared_ptr<Socket>(new Socket(NN_PUB, url));
		auto ret = nn_bind(socket->sid_, socket->url_.bindUrl().c_str());
		if (ret < 0) {

			auto error_str = nn_strerror(nn_errno());
			fprintf(stderr, "socket bind failed: (bind to %s %s)\n", socket->url_().c_str(), error_str);
			throw std::runtime_error(error_str);
		}
		printf("Socket bind: %s\n", socket->url_().data());

		return socket;
	}

	auto Socket::send(const std::string &topic, const std::stringstream &ss) -> unsigned long {

		auto subs = (uint32_t) nn_get_statistic (this->sid_, NN_STAT_CURRENT_CONNECTIONS);
		printf("Socket %s clients %d\n", this->url_().c_str(), subs);

		auto str = topic + '\n' + ss.str();
		auto ret = nn_send(this->sid_, str.c_str(), str.length(), 0);
		if (ret < 0) {

			auto error_str = nn_strerror(nn_errno());
			fprintf(stderr, "socket send failed (%s)\n", error_str);
			throw  std::runtime_error(error_str);
		}

		printf("Socket send: [%s] len %d\n", topic.c_str(), ret);

		return ret - topic.length() - 1;
	}

	auto Socket::Client(const URL &url, unsigned wait) -> std::shared_ptr<Socket> {

		auto socket = std::shared_ptr<Socket>(new Socket(NN_SUB, url, wait));

		auto ret = nn_connect(socket->sid_, socket->url_().c_str());
		if (ret < 0) {

			auto error_str = nn_strerror(nn_errno());
			fprintf(stderr, "socket connect failed (%s)\n", error_str);
			throw std::runtime_error(error_str);
		}
		printf("Socket connect: %s %d\n", socket->url_().c_str(), ret);

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

	auto Socket::startReceive(const std::string &topic, const SsCallback &callback) -> bool {

		this->msg_callback_ = callback;

		{
			auto ret = nn_setsockopt(this->sid_, NN_SUB, NN_SUB_SUBSCRIBE, topic.c_str(), topic.length());
			if (ret < 0) {

				auto error_str = nn_strerror(nn_errno());
				fprintf(stderr, "socket set topic failed (%s)\n", error_str);
				throw std::runtime_error(error_str);
			}
			printf("Socket client set topic: %s\n", topic.data());
		}
		{
			int recv_max_size = -1;
			auto ret = nn_setsockopt(this->sid_, NN_SOL_SOCKET, NN_RCVMAXSIZE, &recv_max_size, sizeof(int));
			if (ret < 0) {

				auto error_str = nn_strerror(nn_errno());
				fprintf(stderr, "socket set recv max size failed (%s)\n", error_str);
				throw std::runtime_error(error_str);
			}
			printf("Socket client set recv max size: %d\n", recv_max_size);
		}

		auto receive_job = [this, topic]() {

			auto head_len = topic.length() + 1;

			while (!this->exit_receive_) {

				void *buf = nullptr;
				auto len = nn_recv(this->sid_, &buf, NN_MSG, 0);
				
				auto recv_topic = std::string((char*)buf);
				recv_topic = recv_topic.substr(0, recv_topic.find('\n'));
				printf("Receiver receive len %d topic %s\n", len, recv_topic.c_str());
				if (len < 0) {

					auto error_str = nn_strerror(nn_errno());
					fprintf(stderr, "socket receive failed (%s)\n", error_str);
					continue;
				}

				const char *msg = static_cast<char*>(buf) + head_len;
				len = len - head_len;

				if (len == sizeof(u_int32_t)) {

					auto code = ntohl(*reinterpret_cast<const uint32_t*>(msg));
					if (code == Socket::CODE_EXIT) {

						printf("Receiver receive exit code %d\n", code);
						break;
					}
				}

				std::stringstream ss;
				ss.write(msg, len);
				this->msg_callback_(ss);

				nn_freemsg(buf);
			}

			printf("Receiver exit\n");
		};

		this->stopReceive();
		this->reveiver_th_ = std::thread(receive_job);

		return true;
	}
}
