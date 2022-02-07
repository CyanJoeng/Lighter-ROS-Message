/**
 * Author: Cyan
 * Date: Thu Dec 23 11:46:10 CST 2021
 */
#include "config.hpp"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <list>

#include <boost/json/src.hpp>
#include <boost/filesystem.hpp>

#include "cmg/core/url.hpp"
#include "cmg/utils/log.hpp"

namespace cmg {

	static auto init_process_port(const boost::json::array &node) -> bool {

		struct IT {

			std::string proc;
			std::string ip;
			std::vector<std::string> topics;
		};

		if (node.empty()) {

			std::list<IT> cfg = {

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

		for (const auto& proc : node) {

			std::string proc_name = proc.at("name").as_string().c_str();
			std::string ip = proc.at("ip").as_string().c_str();

			for (auto topic : proc.at("topics").as_array()) {

//				CMG_DEBUG("[CMG][config] proc %s\t topic %s\t ip %s", proc_name.c_str(), topic.as_string().c_str(), ip.c_str());
				cmg::URL::RegistProc(proc_name, topic.as_string().c_str(), ip);
			}
		}

		return true;
	}

	Config::Config(const std::string &config_file_path) {

		if (config_file_path.empty())
			throw std::runtime_error("[cmg][Config]Config config_file is empty");

		this->root_dir_ = boost::filesystem::path(config_file_path).parent_path().string();

		auto json_file = std::ifstream(config_file_path);
		if (!json_file.is_open())
			throw std::runtime_error("[cmg][Config]Config config_file can not opened");

		std::stringstream ss;
		std::copy(std::istreambuf_iterator<char>(json_file), std::istreambuf_iterator<char>(), std::ostream_iterator<char>(ss));

		try {
			this->data_ = boost::json::parse(ss.str()).as_object();

			auto node_cmg = this->data_.at("cmg");

			const auto& node_log = node_cmg.at("log");

			auto log_level = node_log.at("level").as_string().c_str();
			Log::setLevel(log_level);

			auto log_file = node_log.at("file_name").as_string();
			if (!log_file.empty()) {

				auto log_file_path = boost::filesystem::path(log_file.c_str());
				auto dir_path = log_file_path.parent_path();

				if (!boost::filesystem::exists(dir_path)) {

					CMG_INFO("[Config][log] create log dir");
					boost::filesystem::create_directories(dir_path);
				}

				auto pid = getpid();
				std::string new_name = std::to_string(pid) + "_" + log_file_path.filename().c_str();
				Log::setOutFile((dir_path / new_name).string());
			}

			init_process_port(node_cmg.at("procs").as_array());

		} catch (const std::out_of_range &e) {

			CMG_WARN("[Config][log] error %s", e.what());
		}
	}
}
