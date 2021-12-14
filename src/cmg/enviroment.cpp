/**
 * Author: Cyan
 * Date: Fri Dec  3 12:09:21 CST 2021
 */
#include "enviroment.hpp"

#include <cstdio>
#include <list>
#include <algorithm>
#include <stdexcept>
#include <mutex>
#include <condition_variable>

namespace cmg {

	static std::mutex env_mt;
	static std::condition_variable env_cv;

	std::map<std::string, Environment> Environment::insts;


	Environment::Environment(const std::string &proc_name)
		: proc_name_(proc_name) {}

	Environment::~Environment() {

	}

	auto Environment::Inst(const std::string &proc_name) -> Environment& {

		for (auto [str, env] : insts)
			printf("Environment Inst insts %s, %d\n", str.data(), env.port_);

		printf("Environment Inst proc name %s\n", proc_name.data());

		if (proc_name == Environment::MULTI_PROC_NAME) {

			if (insts.empty())
				throw std::domain_error("Environment not inited with a specific proc_name");
			return insts.begin()->second;
		}

		if (insts.end() == insts.find(proc_name)) {

			{
				std::lock_guard<std::mutex> lck(env_mt);

				auto inst = Environment(proc_name);
				inst.proc_name_ = proc_name;
				insts.emplace(proc_name, inst);
				env_cv.notify_all();
			}

			auto proc_port = Environment::PortFromKey(proc_name);
			printf("Environment Inst proc_name to proc_port %s -> %d\n", proc_name.data(), proc_port);

			insts.at(proc_name).port_ = proc_port;
		}

		return insts.at(proc_name);
	}

	void Environment::Spin() {

		std::unique_lock<std::mutex> lck(env_mt);
		env_cv.wait(lck, []() {

				std::lock_guard<std::mutex> lck(env_mt);

				return Environment::insts.empty();
			});
	}

	auto Environment::PortFromKey(const std::string &key) -> unsigned {

		auto it = URL::proc_ports.find(key);
		if (it == URL::proc_ports.end()) {

			throw std::out_of_range("proc name not registed\n");
		}

		printf("Environment PortFromKey: use key (%s) -> port (%d)\n", key.data(), it->second);

		return it->second;
	}
}
