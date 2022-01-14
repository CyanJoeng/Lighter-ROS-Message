/**
 * Author: Fri Dec 24 18:22:42 CST 2021
 * Date: Fri Dec 24 18:22:49 CST 2021
 */
#pragma once
#include <memory>
#include <vector>

#include "cmg/message/message.hpp"
#include "messages/std_msgs/Header.hpp"

namespace cmg {

	struct StampedFloat64Array : public Message {

		std_msgs::Header header;

		std::vector<double> data;

		virtual auto serialize(std::ostream &out) const -> unsigned long;

		virtual auto parse(std::istream &in) -> unsigned long;
	};

	using StampedFloat64ArrayConstPtr = std::shared_ptr<const StampedFloat64Array>;
}
