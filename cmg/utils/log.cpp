/**
 * Author: Cyan
 * Date: Thu Jan 20 16:54:31 CST 2022
 */
#include "log.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <unistd.h>

namespace cmg {

    std::map<int, LogType> Log::level_ = { };

	std::function<void (std::string)> Log::printer_ = [](const std::string& log) {

		std::cout << log << std::endl;
	};

	void Log::setLevel(const LogType &level) {

        int pid = getpid();
		Log::level_[pid] = level;
		CMG_INFO("[Log][setLevel] pid:%d %s %ld", pid, LogStrOfType[level_[pid]].c_str(), &level_);
	}

	void Log::setLevel(const std::string &level_str) {
		
		auto it = std::find_if(LogStrOfType.begin(), LogStrOfType.end(), [&level_str](auto it) {

				return it.second == level_str;
				});
		Log::setLevel(it->first);
	}

	void Log::setOutFile(const std::string &path) {

		auto log_file = std::make_shared<std::ofstream>(path, std::ios::trunc | std::ios::out);
		if (!log_file->is_open()) {

			CMG_WARN("[Log][setOutFile] can't open log file %s\n", path.c_str());
			return;
		}

		Log::printer_ = [out=log_file](auto log) mutable {

			*out << log << std::endl << std::flush;
		};
	}

	void Log::setPrinter(const std::function<void (const std::string&)> &printer) {

		Log::printer_ = printer;
	}

	Log::~Log() {

        int pid = getpid();
		if (this->type_ < Log::level_[pid])
			return;
		Log::printer_(this->str());
	}
}
