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

#include <opencv2/core.hpp>

namespace cmg {

	static std::mutex env_mt;
	static std::condition_variable env_cv;

	std::list<Environment> Environment::env_insts_;

	std::shared_ptr<cmg::Config> Environment::config_ = nullptr;


	Environment::Environment(const std::string &proc_name)
		: proc_name_(proc_name) {}

	Environment::~Environment() {}

	auto Environment::Inst(const std::string &proc_name) -> Environment& {

		if (proc_name == Environment::MULTI_PROC_NAME) {

			if (env_insts_.empty())
				throw std::domain_error("Environment not inited with a specific proc_name");
			return env_insts_.front();
		}

		auto it = std::find_if(Environment::env_insts_.begin(), Environment::env_insts_.end(), [&proc_name](auto &it) {
				return it.proc_name_ == proc_name;
				});

		if (env_insts_.end() != it)
			return *it;

		std::lock_guard<std::mutex> lck(env_mt);

		printf("[Environment][Inst] create env with proc_name %s\n", proc_name.c_str());
		Environment::env_insts_.push_back(Environment(proc_name));
		env_cv.notify_all();

		return env_insts_.back();
	}

	void Environment::Spin() {

		std::unique_lock<std::mutex> lck(env_mt);
		env_cv.wait(lck, []() {

				std::lock_guard<std::mutex> lck(env_mt);

				return Environment::env_insts_.empty();
			});
	}

	void Environment::Shutdown() {

		{
			std::lock_guard<std::mutex> lck(env_mt);
			Environment::env_insts_.clear();
		}
		env_cv.notify_all();
	}

	auto Environment::Config(const std::string &config_file) -> bool {

		try {

			Environment::config_ = std::make_shared<cmg::Config>(config_file);
			return true;
		} catch (const std::exception &e) {

			printf("[Environment]Config Exception: %s\n", e.what());
			printf("[Environment]Config config_file open failed: %s\n", config_file.c_str());
			Environment::config_ = nullptr;
			return false;
		}
	}

	auto Environment::Config() -> const cmg::Config& {

		if (Environment::config_ == nullptr)
			throw std::runtime_error("[Environment]Config config not be set correctly");
		return * Environment::config_;
	}
}
