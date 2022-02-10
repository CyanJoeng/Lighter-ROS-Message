/**
 * Author: Cyan
 * Date: Thu Jan 20 16:54:31 CST 2022
 */
#include "log.hpp"

#include <fstream>
#include <iostream>
#include <memory>

namespace cmg {

	LogType Log::level_ = []() {

		printf("Set Log to WARN Inited\n");
		return LogType::WARN;
	}();

	std::function<void (std::string)> Log::printer_ = [](const std::string& log) {

		std::cout << log << std::endl;
	};

	void Log::setLevel(const LogType &level) {

		Log::level_ = level;
		CMG_INFO("[Log][setLevel] %s %ld", LogStrOfType[level_].c_str(), &level_);
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

	void Log::setPrinter(const std::function<void (const std::string&)> &printer, LogType level) {

		Log::printer_ = printer;
		Log::setLevel(level);
	}

	Log::~Log() {

		if (this->type_ < Log::level_)
			return;
		Log::printer_(this->str());
	}
}
