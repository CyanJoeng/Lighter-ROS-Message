/**
 * Author: Cyan
 * Date: Fri Dec 17 14:31:44 CST 2021
 */
#include "url.hpp"
#include <map>
#include <mutex>
#include <sstream>
#include <stdexcept>

#include "cmg/utils/log.hpp"

namespace cmg {

	static constexpr unsigned BASE_PORT = 39527;

	static constexpr auto PROTO = "tcp://";

	static constexpr auto LOCAL_IP = "0.0.0.0";


	std::map<std::string, URL> URL::insts_;

	std::mutex URL::inst_mt_;

	auto URL::Inst(const std::string &name, const std::string &topic) -> URL& {

		std::lock_guard<std::mutex> lck(URL::inst_mt_);
		auto it = URL::insts_.find(URL::Key(name, topic));
		if (URL::insts_.end() == it) {

			std::stringstream ss;
			ss << "Process not registered: " << URL::Key(name, topic);
			throw std::out_of_range(ss.str());
		}

		return it->second;
	};

	auto URL::RegisterProc(const std::string &proc_name, const std::string &topic, const std::string &address) -> URL& {

		std::lock_guard<std::mutex> lck(URL::inst_mt_);

		auto &new_inst = URL::insts_[URL::Key(proc_name, topic)];
		new_inst.proc_name_ = proc_name;
		new_inst.topic_ = topic;
		new_inst.net_port_ = BASE_PORT + URL::insts_.size();
		new_inst.tcp_url_ = URL::BuildUrl(address, new_inst.net_port_);

		CMG_INFO("[URL]register /%s/%s -> %s", new_inst.proc_name_.c_str(), topic.c_str(), new_inst.tcp_url_.c_str());

		return new_inst;
	}

    auto URL::BuildUrl(const std::string &address, unsigned port) -> std::string {

		return PROTO + address + ":" + std::to_string(port);
	}

	auto URL::Key(const std::string& proc_name, const std::string &topic) -> std::string {

		return proc_name + "," + topic;
	}


	auto URL::name() const  -> const std::string& {
		return this->proc_name_;
	}

	auto URL::topic() const -> const std::string & {
		return this->topic_;
	}

	auto URL::port() const -> unsigned {
		return this->net_port_;
	}

	auto URL::operator() () const -> const std::string& {
		return this->tcp_url_;
	}

	auto URL::bindUrl() const -> std::string {

		return URL::BuildUrl(LOCAL_IP, this->net_port_);
	}
}
