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

	std::map<Environment::ProcType, Environment> Environment::insts;


	Environment::Environment(const std::string &proc_name)
		: proc_name_(proc_name) {}

	Environment::~Environment() {}

	auto Environment::Inst(const std::string &proc_name) -> Environment& {

		if (proc_name == Environment::MULTI_PROC_NAME) {

			if (insts.empty())
				throw std::domain_error("Environment not inited with a specific proc_name");
			return insts.begin()->second;
		}

		if (insts.end() == insts.find(proc_name)) {

			std::lock_guard<std::mutex> lck(env_mt);

			insts.emplace(proc_name, Environment(proc_name));

			env_cv.notify_all();
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

	void Environment::Shutdown() {

		{
			std::lock_guard<std::mutex> lck(env_mt);
			Environment::insts.clear();
		}
		env_cv.notify_all();
	}
}
