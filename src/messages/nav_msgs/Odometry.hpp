/**
 * Author: Cyan
 * Date: Wed Dec 22 15:50:57 CST 2021
 */
#pragma once

#include "messages/message.hpp"
#include "messages/std_msgs/Header.hpp"
#include <array>
#include <memory>
#include <vector>

namespace cmg { namespace nav_msgs {

	struct Odometry : public cmg::Message {

		std_msgs::Header header;

		struct {

			std::array<double, 3> position;
			std::array<double, 4> orientation;

		} pose;

		std::array<double, 3> twist;

		virtual auto serialize(std::ostream &out) -> unsigned long final;

		virtual auto parse(std::istream &in) -> unsigned long final;

	};

	using OdometryPtr = std::shared_ptr<Odometry>;

}}
