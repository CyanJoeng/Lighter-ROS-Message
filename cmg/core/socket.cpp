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

#include "cmg/utils/log.hpp"


namespace cmg {

	Socket::Socket(int protocol, const URL &url, unsigned wait)
		: url_(url) {

		this->sid_ = nn_socket(AF_SP, protocol);
		if (this->sid_ < 0) {

			auto error_str = nn_strerror(nn_errno());
			CMG_ERROR("socket open failed: (%s)", error_str);
			throw std::runtime_error(error_str);
		}
	};

	Socket::~Socket() {

		nn_close(this->sid_);
		this->stopReceive();
	}

	auto Socket::Server(const URL &url) -> std::shared_ptr<Socket> {

		auto socket = std::shared_ptr<Socket>(new Socket(NN_PUB, url));

		auto bind_url = socket->url_.bindUrl();
		auto ret = nn_bind(socket->sid_, bind_url.c_str());
		if (ret < 0) {

			auto error_str = nn_strerror(nn_errno());
			CMG_ERROR("socket bind failed: (bind to %s %s)", socket->url_().c_str(), error_str);
			throw std::runtime_error(error_str);
		}
		CMG_INFO("[Socket](Server) bind: %s (%s)", bind_url.c_str(), socket->url_.topic().c_str());

		return socket;
	}

	auto Socket::send(const std::string &topic, const std::stringstream &ss) -> unsigned long {

		//auto subs = (uint32_t) nn_get_statistic (this->sid_, NN_STAT_CURRENT_CONNECTIONS);
		//CMG_DEBUG("Socket %s clients %d", this->url_().c_str(), subs);

		auto str = ss.str();
		auto ret = 0;
		{
			std::lock_guard<std::mutex> lck(this->msg_send_mt_);
			ret = nn_send(this->sid_, str.c_str(), str.length(), 0);
		}
		if (ret < 0) {

			auto error_str = nn_strerror(nn_errno());
			CMG_ERROR("socket send failed (%s)", error_str);
			throw  std::runtime_error(error_str);
		}

		//CMG_DEBUG("Socket send: [%s] len %d", topic.c_str(), ret);

		return ret;
	}

	auto Socket::Client(const URL &url, unsigned wait) -> std::shared_ptr<Socket> {

		auto socket = std::shared_ptr<Socket>(new Socket(NN_SUB, url, wait));

		auto ret = nn_connect(socket->sid_, socket->url_().c_str());
		if (ret < 0) {

			auto error_str = nn_strerror(nn_errno());
			CMG_ERROR("socket connect failed (%s)", error_str);
			throw std::runtime_error(error_str);
		}
		CMG_INFO("[Socket](Client) connect: %s", socket->url_().c_str());

		return socket;
	}

	auto Socket::stopReceive() -> bool {

		this->exit_receive_ = true;
		if (this->reveiver_th_ && this->reveiver_th_->joinable()) {

			int timeout = 1;
			nn_setsockopt(this->sid_, NN_SUB, NN_RCVTIMEO, &timeout, sizeof(int));
			this->reveiver_th_->join();
		}
		this->exit_receive_ = false;

		return true;
	}

	auto Socket::startReceive(const std::string &topic, const SsCallback &callback) -> bool {

		this->msg_callback_ = callback;

		std::string use_topic;

		{
			auto ret = nn_setsockopt(this->sid_, NN_SUB, NN_SUB_SUBSCRIBE, use_topic.c_str(), use_topic.length());
			if (ret < 0) {

				auto error_str = nn_strerror(nn_errno());
				CMG_ERROR("socket set topic failed (%s)", error_str);
				throw std::runtime_error(error_str);
			}
			CMG_INFO("[Socket](startReceive) client set topic: %s", topic.data());
		}
		{
			int recv_max_size = -1;
			auto ret = nn_setsockopt(this->sid_, NN_SOL_SOCKET, NN_RCVMAXSIZE, &recv_max_size, sizeof(int));
			if (ret < 0) {

				auto error_str = nn_strerror(nn_errno());
				CMG_ERROR("socket set recv max size failed (%s)", error_str);
				throw std::runtime_error(error_str);
			}
			CMG_INFO("[Socket](startReceive) client set recv max size: %d", recv_max_size);
		}

		auto receive_job = [this, use_topic]() {

			std::string tag = this->url_.name() + "/" + this->url_.topic();

			while (true) {

				if (this->exit_receive_) {
					break;
				}

				void *buf = nullptr;
				auto len = nn_recv(this->sid_, &buf, NN_MSG, 0);
				
				if (use_topic.length()) {

					auto recv_topic = std::string((char*)buf);
					recv_topic = recv_topic.substr(0, recv_topic.find('\n'));
					CMG_INFO("[Socket](receive_job)%s: receive len %d topic %s", tag.c_str(), len, recv_topic.c_str());
				}
				if (len < 0) {

					auto error_str = nn_strerror(nn_errno());
					CMG_ERROR("[Socket](receive_job)%s: receive failed (%s)", tag.c_str(), error_str);
					continue;
				}

				const char *msg = static_cast<char*>(buf);

				if (len == sizeof(u_int32_t)) {

					auto code = ntohl(*reinterpret_cast<const uint32_t*>(msg));
					if (code == Socket::CODE_EXIT) {

						CMG_INFO("[Socket](receive_job)%s: receive exit code %d", tag.c_str(), code);
						break;
					}
				}

				std::stringstream ss;
				ss.write(msg, len);
				this->msg_callback_(ss);

				nn_freemsg(buf);
			}

			CMG_INFO("[Socket](receive_job)%s: exit", tag.c_str());
		};

		this->stopReceive();
		this->reveiver_th_ = std::make_shared<std::thread>(receive_job);

		return true;
	}
}
