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

		std::string tag_;

	public:
		Log(LogType type, const std::string &tag) : type_(type), tag_("{" + tag + "}") {}

		Log(const Log &inst) {

			this->str(inst.str());
		}

		~Log();

		auto operator() () -> Log& {

			*this << "(" << LogStrOfType[type_] << ") " << tag_ << " ";
			return *this;
		}

		template <typename... Args>
		void operator() (const char *fmt, Args... args) {

		    if (this->type_ < Log::level_)
				return;

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


		static auto Info(const std::string &tag) -> Log { return {LogType::INFO, tag}; }

		static auto Debug(const std::string &tag) -> Log { return {LogType::DEBUG, tag}; }

		static auto Warn(const std::string &tag) -> Log { return {LogType::WARN, tag}; }

		static auto Error(const std::string &tag) -> Log { return {LogType::ERROR, tag}; }
	};
}

#define TAG ""

#define CMG_INFO_STREAM cmg::Log::Info(TAG)
#define CMG_DEBUG_STREAM cmg::Log::Debug(TAG)
#define CMG_WARN_STREAM cmg::Log::Warn(TAG)
#define CMG_ERROR_STREAM cmg::Log::Error(TAG)


#define CMG_INFO cmg::Log {cmg::LogType::INFO, TAG}
#define CMG_DEBUG cmg::Log {cmg::LogType::DEBUG, TAG}
#define CMG_WARN cmg::Log {cmg::LogType::WARN, TAG}
#define CMG_ERROR cmg::Log {cmg::LogType::ERROR, TAG}
