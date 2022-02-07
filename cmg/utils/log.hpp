/**
 * Author Cyan
 * Date: Thu Dec 23 17:14:51 CST 2021
 */
#pragma once

#include <tuple>
#include <vector>
#include <map>
#include <sstream>
#include <functional>

namespace cmg {

	enum class LogType {

		DEBUG=0, INFO, WARN, ERROR
	};

	static std::map<LogType, std::string> LogStrOfType = {
		{LogType::DEBUG, "Debug"},
		{LogType::INFO, "Info"},
		{LogType::WARN, "Warn"},
		{LogType::ERROR, "Error"}
	};


	class Log : public std::stringstream {

		class OutFile;

		static LogType level_;

		static std::function<void (std::string)> printer_;

		LogType type_;

	public:
		Log(LogType type) : type_(type) {}

		Log(const Log &inst) {

			this->str(inst.str());
		}

		~Log();

		auto operator() () -> Log& {

			*this << "[CMG](" << LogStrOfType[type_] << ") ";
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

		static void setLevel(const LogType &tlevel);

		static void setLevel(const std::string &level_str);

		static void setOutFile(const std::string &path);

		static void setPrinter(const std::function<void (const std::string&)> &printer, LogType level = Log::level_);


		static auto Info() -> Log { return {LogType::INFO}; }

		static auto Debug() -> Log { return {LogType::DEBUG}; }

		static auto Warn() -> Log { return {LogType::WARN}; }

		static auto Error() -> Log { return {LogType::ERROR}; }
	};
}

#define CMG_INFO_STREAM cmg::Log::Info()
#define CMG_DEBUG_STREAM cmg::Log::Debug()
#define CMG_WARN_STREAM cmg::Log::Warn()
#define CMG_ERROR_STREAM cmg::Log::Error()


#define CMG_INFO cmg::Log {cmg::LogType::INFO}
#define CMG_DEBUG cmg::Log {cmg::LogType::DEBUG}
#define CMG_WARN cmg::Log {cmg::LogType::WARN}
#define CMG_ERROR cmg::Log {cmg::LogType::ERROR}
