/**
 * Author: Cyan
 * Date: Wed Dec  8 00:04:31 CST 2021
 */
#pragma once
#include <memory>
#include <vector>

#include "messages/message.hpp"
#include "messages/std_msgs/Header.hpp"
#include "messages/geometry_msgs/point32.hpp"

namespace cmg { namespace sensor_msgs {

	struct ChannelFloat32 {

		std::vector<float> values;
	};

	struct PointCloud : public cmg::Message {

		std_msgs::Header header;

		std::vector<geometry_msgs::Point32> points;

		std::vector<ChannelFloat32> channels;

	public:
		virtual auto serialize(std::ostream &out) const -> unsigned long final;

		virtual auto parse(std::istream &in) -> unsigned long final;
	};


	using PointCloudPtr = std::shared_ptr<PointCloud>;
	using PointCloudConstPtr = std::shared_ptr<const PointCloud>;
}}
