/**
 * Author: Cyan
 * Date: Thu Dec 23 11:46:10 CST 2021
 */
#include "config.hpp"

#include <stdexcept>
#include <vector>
#include <list>

#include <opencv2/core/persistence.hpp>

#include "cmg/url.hpp"

namespace cmg {

	static auto init_process_port(const cv::FileNode &node) -> bool {

		struct _IT {

			std::string proc;
			std::string ip;
			std::vector<std::string> topics;
		};

		if (node.size() == 0) {

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

			std::string proc_name = proc["name"];
			std::string ip = proc["ip"];

			for (auto topic : proc["topics"]) {

				//printf("[CMG][config] proc %s\t topic %s\t ip %s\n", proc_name.c_str(), topic.c_str(), ip.c_str());
				cmg::URL::RegistProc(proc_name, topic, ip);
			}
		}

		return true;
	}

	Config::Config(const std::string &file_path) {

		if (file_path.empty())
			throw std::runtime_error("[cmg][Config]Config config_file is empty");

		auto cfg = cv::FileStorage(file_path, cv::FileStorage::READ | cv::FileStorage::FORMAT_JSON);
		if (!cfg.isOpened())
			throw std::runtime_error("[cmg][Config]Config config_file can not opened");

		auto cmg = cfg["cmg"];
		init_process_port(cmg["procs"]);

		auto root = cfg.root();
		for (auto it : root) {

			this->emplace(it.name(), it);
		}
	}
}
