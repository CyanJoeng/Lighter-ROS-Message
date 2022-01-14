/**
 * Author: Cyan
 * Date: Fri Dec 24 17:45:06 CST 2021
 */
#pragma once

#include "cmg/message/message.hpp"
#include "messages/std_msgs/Header.hpp"

namespace cmg { namespace sensor_msgs {

	struct NavSatFix : public Message {

		std_msgs::Header header;

		double latitude;
		double longitude;
		double altitude;

		virtual auto serialize(std::ostream &out) const -> unsigned long;

		virtual auto parse(std::istream &in) -> unsigned long;
	};
}}
