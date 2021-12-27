/**
 * Author: Cyan
 * Date: Fri Dec 24 10:42:52 CST 2021
 */
#pragma once

#include <memory>

#include "messages/message.hpp"
#include "messages/std_msgs/Header.hpp"

namespace cmg { namespace sensor_msgs {

	struct Imu : public Message {

		std_msgs::Header header;

		std_msgs::Point3 linear_acceleration;
		
		std_msgs::Point3 angular_velocity;

	public:
		virtual auto serialize(std::ostream &out) const -> unsigned long;

		virtual auto parse(std::istream &in) -> unsigned long;
	};

	using ImuPtr = std::shared_ptr<Imu>;
	using ImuConstPtr = std::shared_ptr<const Imu>;
}}
