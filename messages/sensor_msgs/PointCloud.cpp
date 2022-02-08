/**
 * Author: Cyan
 * Date: Wed Dec  8 00:11:40 CST 2021
 */
#include "PointCloud.hpp"
#include <cstdio>

#include "sensor_msgs.pb.h"

namespace cmg { namespace sensor_msgs {

	auto PointCloud::serialize(std::ostream &out) const -> unsigned long {

		cmg_pb::PointCloud msg;

		auto header = msg.mutable_header();
		header->set_stamp_sec(this->header.stamp.toSec());
		header->set_frame_id(this->header.frame_id);

		for (auto &pt : this->points) {

			auto point = msg.add_points();
			point->set_x(pt.x);
			point->set_y(pt.y);
			point->set_z(pt.z);
		}

		for (auto ch : this->channels) {

			auto channel = msg.add_channels();
			for (auto val : ch.values) {
				channel->add_vals(val);
            }
		}

		if (msg.SerializePartialToOstream(&out))
			return msg.ByteSizeLong();
		else
			CMG_WARN("PointCloud serialize failed\n");
		return 0;
	}

	auto PointCloud::parse(std::istream &in) -> unsigned long {

		cmg_pb::PointCloud msg;

		if (!msg.ParseFromIstream(&in)) {

			CMG_WARN("PointCloud parse failed\n");
			return 0;
		}

		this->header.stamp.time_ = msg.header().stamp_sec();
		this->header.frame_id = msg.header().frame_id();

		this->points.reserve(msg.points_size());
		for (auto &pt : msg.points()) {

			geometry_msgs::Point32 p;
			p.x = pt.x();
			p.y = pt.y();
			p.z = pt.z();
			this->points.push_back(p);
		}

		this->channels.reserve(msg.channels_size());
		for (auto &ch : msg.channels()) {

			ChannelFloat32 channel;
			channel.values.reserve(ch.vals_size());
			for (auto val : ch.vals()) {

				channel.values.push_back(val);
			}

			this->channels.push_back(channel);
		}

		return msg.ByteSizeLong();
	}
}}
