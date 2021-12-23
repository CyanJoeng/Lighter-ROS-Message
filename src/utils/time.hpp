/**
 * Author: Cyan
 * Date: Thu Dec 23 18:44:25 CST 2021
 */
#pragma once

#include <chrono>
namespace cmg {


	class Time {

	public:
		struct TimePoint : public std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> {

			using TP = std::chrono::time_point<std::chrono::steady_clock>;

			TimePoint(const TP &t) {

				*this = t;
			}

			auto toSec() -> double {

				return this->time_since_epoch().count() * 1e-3;
			}
		};

	public:
		static auto now() -> TimePoint {

			return std::chrono::steady_clock::now();
		}
	};
}
