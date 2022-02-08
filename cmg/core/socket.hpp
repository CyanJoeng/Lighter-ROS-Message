/**
 * Author: Cyan
 * Date: Thu Dec  2 16:56:57 CST 2021
 */
#pragma once
#include <mutex>
#include <sstream>
#include <thread>
#include <memory>
#include <map>
#include <functional>

#include "url.hpp"

namespace cmg {

    class Socket {

        static constexpr auto CODE_EXIT = 4399;

    public:
		using SsCallback = std::function<void (std::stringstream &)>;
		
	private:
		int sid_;

		const URL& url_;

		SsCallback msg_callback_;

		std::shared_ptr<std::thread> reveiver_th_;

		std::mutex msg_send_mt_;

		bool exit_receive_ {false};

	private:
		Socket(int protocol, const URL &url, unsigned wait = 0);

	public:
		~Socket();

		// Server port
		static auto Server(const URL &url) -> std::shared_ptr<Socket>;

		auto send(const std::string &topic, const std::stringstream &str) -> unsigned long;

		// client port
		static auto Client(const URL &url, unsigned wait = 0) -> std::shared_ptr<Socket>;

		auto stopReceive() -> bool;

		auto startReceive(const std::string &topic, const SsCallback &callback) -> bool;
	};
}
