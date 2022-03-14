/**
 * Author: Cyan
 * Date: Fri Dec 17 14:29:50 CST 2021
 */
#pragma once
#include <mutex>
#include <string>
#include <map>

namespace cmg {

	/**
	 * charge of all definition of socket
	 * proc_name <--> port <--> url
	 */
	class URL {

	public:
		static auto Inst(const std::string& proc_name, const std::string &topic) -> URL&;

		static auto RegisterProc(const std::string &proc_name, const std::string &topic, const std::string &address) -> URL&;

		static auto BuildUrl(const std::string &address, unsigned port) -> std::string;

	private:
		static auto Key(const std::string& proc_name, const std::string &topic) -> std::string;

	private:
		static std::map<std::string, URL> insts_;

		static std::mutex inst_mt_;

	public:
		auto name() const -> const std::string&;

		auto topic() const -> const std::string&;

		auto port() const -> unsigned;

		auto operator() () const -> const std::string&;

		auto bindUrl() const -> std::string;

	private:
		std::string proc_name_;

		std::string topic_;

		unsigned net_port_;

		std::string tcp_url_;
	};
}

