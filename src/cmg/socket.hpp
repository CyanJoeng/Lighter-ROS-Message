/**
 * Author: Cyan
 * Date: Thu Dec  2 16:56:57 CST 2021
 */
#pragma once
#include <sstream>
#include <thread>
#include <map>
#include <functional>

namespace cmg {

	struct URL {

		static constexpr unsigned BASE_PORT = 9527;

		static std::map<std::string, unsigned> proc_ports;

		static constexpr auto LOCAL_SOCKET = "tcp://0.0.0.0";

		static auto Build(const std::string &address, unsigned port) -> const std::string;
	};

	class Socket {

		static constexpr auto CODE_EXIT = 4399;

	public:
		using SsCallback = std::function<void (std::stringstream &)>;
		
	private:
		int sid_;

		std::string url_;

		SsCallback msg_callback_;

		std::thread reveiver_th_;
		bool exit_receive_{false};

	private:
		Socket(int protocol, unsigned port, const std::string &address, unsigned wait = 0);

	public:
		~Socket();

		// Server port
		static auto Server(unsigned port, const std::string &address = URL::LOCAL_SOCKET) -> std::shared_ptr<Socket>;

		auto send(const std::string &topic, const std::stringstream &str) -> unsigned long;

		// client port
		static auto Client(unsigned port, const std::string &address = URL::LOCAL_SOCKET, unsigned wait = 0) -> std::shared_ptr<Socket>;

		auto stopReceive() -> bool;

		auto startReceive(const std::string &topic, const SsCallback &callback) -> bool;
	};
}
