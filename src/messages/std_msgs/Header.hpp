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

			auto toSec() -> double { return this->time_; }
		} stamp;

		std::string frame_id{""};
	};
}}
