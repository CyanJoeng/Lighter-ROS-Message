/**
 * Author: Cyan
 * Date: Wed Dec 22 15:50:57 CST 2021
 */
#pragma once

#include <array>
#include <memory>
#include <vector>

#include "cmg/message/message.hpp"
#include "messages/std_msgs/Header.hpp"

namespace cmg { namespace nav_msgs {

	struct Odometry : public cmg::Message {

		std_msgs::Header header;

		struct {

			struct {
				double x, y, z;
			} position;

			struct {
				double x, y, z, w;
			} orientation;

		} pose;

		struct {
			double x, y, z;

		} twist;

		virtual auto serialize(std::ostream &out) const -> unsigned long final;

		virtual auto parse(std::istream &in) -> unsigned long final;

	};

	using OdometryPtr = std::shared_ptr<Odometry>;

	using OdometryConstPtr = std::shared_ptr<const Odometry>;

}}
