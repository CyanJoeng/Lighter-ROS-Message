/**
 * Author: Cyan
 * Date: Fri Dec  3 12:09:21 CST 2021
 */
#include "environment.hpp"

#include <cstdio>
#include <list>
#include <mutex>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <condition_variable>

#include <boost/algorithm/string.hpp>


namespace cmg {

	static std::mutex env_mt;
	static std::condition_variable env_cv;

	std::list<Environment> Environment::env_insts_;

	std::shared_ptr<cmg::Config> Environment::config_ = nullptr;


	Environment::Environment(std::string proc_name)
		: proc_name_(std::move(proc_name)) {}

	Environment::~Environment() = default;

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

		CMG_INFO("[Environment][Inst] create env with proc_name %s", proc_name.c_str());
		Environment::env_insts_.push_back(Environment(proc_name));
		env_cv.notify_all();

		return env_insts_.back();
	}

	void Environment::Spin() {

		std::unique_lock<std::mutex> lck(env_mt);
		env_cv.wait(lck, []() {

				return Environment::env_insts_.empty();
			});
		lck.unlock();
	}

	void Environment::Close(const Environment &env) {

	    auto &envs = Environment::env_insts_;
		auto it = std::find_if(envs.begin(), envs.end(), [&env](auto it) {
			return it.proc_name_ == env.proc_name_;
		});

		if (it != envs.end())
		{
			std::lock_guard<std::mutex> lck(env_mt);
			envs.erase(it);
		}
		env_cv.notify_all();
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

			CMG_WARN("[Environment]Config Exception: %s", e.what());
			CMG_WARN("[Environment]Config config_file open failed: %s", config_file.c_str());
			Environment::config_ = nullptr;
			return false;
		}
	}

	auto Environment::Config() -> cmg::Config& {

		if (Environment::config_ == nullptr)
			throw std::runtime_error("[Environment]Config config not be set correctly");
		return * Environment::config_;
	}

    auto Environment::SplitNameTopic(const std::string &proc_name_topic) -> std::vector<std::string> {

        std::vector<std::string> strs;
        boost::split(strs, proc_name_topic, boost::is_any_of("/"));

        if (strs.empty())
            return strs;

        return {strs.begin() + 1, strs.end()};
    }
}
