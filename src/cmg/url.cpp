/**
 * Author: Cyan
 * Date: Fri Dec 17 14:31:44 CST 2021
 */
#include "url.hpp"
#include <map>
#include <mutex>
#include <sstream>
#include <stdexcept>

namespace cmg {

	std::map<std::string, URL> URL::insts_;

	std::mutex URL::inst_mt_;

	auto URL::Inst(const std::string &name) -> URL& {

		std::lock_guard<std::mutex> lck(URL::inst_mt_);
		auto it = URL::insts_.find(name);
		if (URL::insts_.end() == it) {

			std::stringstream ss;
			ss << "Process not registed: " << name;
			throw std::out_of_range(ss.str());
		}

		return it->second;
	};

	auto URL::RegistProc(const std::string &proc_name, const std::string &address) -> URL& {

		std::lock_guard<std::mutex> lck(URL::inst_mt_);

		auto &new_inst = URL::insts_[proc_name];
		new_inst.proc_name_ = proc_name;
		new_inst.net_port_ = BASE_PORT + URL::insts_.size();
		new_inst.tcp_url_ = URL::BuildUrl(address, new_inst.net_port_);

		printf("[URL]regist %s %d %s\n", new_inst.proc_name_.c_str(), new_inst.net_port_, new_inst.tcp_url_.c_str());

		return new_inst;
	}

    auto URL::BuildUrl(const std::string &address, unsigned port) -> const std::string {

		return URL::PROTO + address + ":" + std::to_string(port);
	}


	auto URL::name() const  -> const std::string& {
		return this->proc_name_;
	}

	auto URL::port() const -> const unsigned {
		return this->net_port_;
	}

	auto URL::operator() () const -> const std::string& {
		return this->tcp_url_;
	}

	auto URL::bindUrl() const -> const std::string {

		return URL::BuildUrl(URL::LOCAL_IP, this->net_port_);
	}
}
