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

			using TP = std::chrono::time_point<std::chrono::system_clock>;

			TP tp_;

			TimePoint(const TP &tp) : tp_(tp) {}

			auto toSec() const -> double {

				return std::chrono::duration<double>(
						this->tp_.time_since_epoch()).count();
			}
		};

	public:
		static auto now() -> TimePoint {

			return std::chrono::system_clock::now();
		}

        static auto diff(const TimePoint &_1, const TimePoint &_2) -> double {

            return _2.toSec() - _1.toSec();
        }
	};
}
