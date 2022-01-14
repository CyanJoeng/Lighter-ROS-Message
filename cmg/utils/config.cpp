/**
 * Author: Cyan
 * Date: Thu Dec 23 11:46:10 CST 2021
 */
#include "config.hpp"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <list>

#include <boost/json/src.hpp>

#include "cmg/core/url.hpp"

namespace cmg {

	static auto init_process_port(const boost::json::array &node) -> bool {

		struct _IT {

			std::string proc;
			std::string ip;
			std::vector<std::string> topics;
		};

		if (node.empty()) {

			std::list<_IT> cfg = {

				{"station", "172.18.135.103", {
												  "match",
												  "foo"
											  }},
				{"client", "0.0.0.0", {}}
			};

			for (auto [proc_name, ip, topics] : cfg)
				for (auto &topic : topics)
					cmg::URL::RegistProc(proc_name, topic, ip);


			return true;
		}

		for (auto proc : node) {

			std::string proc_name = proc.at("name").as_string().c_str();
			std::string ip = proc.at("ip").as_string().c_str();

			for (auto topic : proc.at("topics").as_array()) {

				//printf("[CMG][config] proc %s\t topic %s\t ip %s\n", proc_name.c_str(), topic.c_str(), ip.c_str());
				cmg::URL::RegistProc(proc_name, topic.as_string().c_str(), ip);
			}
		}

		return true;
	}

	Config::Config(const std::string &file_path) {

		if (file_path.empty())
			throw std::runtime_error("[cmg][Config]Config config_file is empty");

		auto json_file = std::ifstream(file_path);
		if (!json_file.is_open())
			throw std::runtime_error("[cmg][Config]Config config_file can not opened");

		std::stringstream ss;
		std::copy(std::istreambuf_iterator<char>(json_file), std::istreambuf_iterator<char>(), std::ostream_iterator<char>(ss));

		this->data_ = boost::json::parse(ss.str());

		auto cmg = this->data_.at("cmg");
		init_process_port(cmg.at("procs").as_array());
	}
}
