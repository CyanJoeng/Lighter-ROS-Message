/**
 * Author Cyan
 * Date: Thu Dec 23 17:14:51 CST 2021
 */
#pragma once

#include <cstdio>
#include <functional>
#include <tuple>
#include <vector>
#include <map>
#include <sstream>
namespace cmg {

	enum class LogType {

		INFO=0, DEBUG, WARN, ERROR
	};

	static std::map<LogType, std::string> LogStrOfType = {
		{LogType::INFO, "Info"},
		{LogType::DEBUG, "Debug"},
		{LogType::WARN, "Warn"},
		{LogType::ERROR, "Error"}
	};

	class Log : public std::stringstream {

		LogType type_;

	public:
		Log(LogType type) : type_(type) {}

		~Log() {

			printf("%s\n", this->str().c_str());
		}

		auto operator() () -> Log& {

			*this << "[CMG]" << LogStrOfType[type_] << " ";
			return *this;
		}

		template <typename... Args>
		void operator() (const char *fmt, Args... args) {

			(*this)();

			std::string buf;
			buf.resize(256);

			std::apply(sprintf, std::tuple_cat(std::make_tuple(buf.data(), fmt), std::make_tuple(args...)));

			*this << buf;
		}

		static auto Info() -> Log { return Log {LogType::INFO}; }

		static auto Debug() -> Log { return Log {LogType::DEBUG}; }

		static auto Warn() -> Log { return Log {LogType::WARN}; }

		static auto Error() -> Log { return Log {LogType::ERROR}; }
	};
}

#define CMG_INFO_STREAM cmg::Log::Info
#define CMG_DEBUG_STREAM cmg::Log::Debug
#define CMG_WARN_STREAM cmg::Log::Warn
#define CMG_ERROR_STREAM cmg::Log::Error


#define CMG_INFO cmg::Log {cmg::LogType::INFO}
#define CMG_DEBUG cmg::Log {cmg::LogType::DEBUG}
#define CMG_WARN cmg::Log {cmg::LogType::WARN}
#define CMG_ERROR cmg::Log {cmg::LogType::ERROR}
