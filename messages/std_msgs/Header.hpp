/**
 * Author: Cyan
 * Date: Wed Dec  8 10:47:46 CST 2021
 */
#pragma once

#include <string>

namespace cmg { namespace std_msgs {

	struct Header {

		struct {
			
			double time_{0.0};

			auto toSec() const -> double { return this->time_; }

			double operator= (const double t) {

				return this->time_ = t;
			}
		} stamp;

		std::string frame_id{""};
	};


	struct Point3 {

		double x, y, z;
	};
}}
