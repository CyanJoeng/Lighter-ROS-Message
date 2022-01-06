/**
 * Author: Cyan
 * Date: Thu Dec 23 18:44:25 CST 2021
 */
#pragma once

#include <chrono>
namespace cmg {


	class Time {

	public:
		struct TimePoint {

			using TP = std::chrono::time_point<std::chrono::steady_clock>;

			TP tp_;

			TimePoint(const TP &tp) : tp_(tp) {}

			auto toSec() -> double {

				return std::chrono::duration_cast<std::chrono::seconds>(
						this->tp_.time_since_epoch()).count();
			}
		};

	public:
		static auto now() -> TimePoint {

			return std::chrono::steady_clock::now();
		}
	};
}
