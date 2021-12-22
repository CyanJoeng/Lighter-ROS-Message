/**
 * Author: Cyan
 * Date: Wed Dec 22 16:09:59 CST 2021
 */
#include "Odometry.h"

#include <sstream>

#include "protos/nav_msgs.pb.h"
#include "messages/codex.hpp"


namespace cmg { namespace nav_msgs {

	auto Odometry::serialize(std::ostream &out) -> unsigned long {

		cmg_pb::Odometry msg;

		auto header = msg.mutable_header();
		header->set_stamp_sec(this->header.stamp.toSec());
		header->set_frame_id(this->header.frame_id);

		auto pose = msg.mutable_pose();
		auto position = pose->mutable_position();
		position->set_x(this->pose.position[0]);
		position->set_y(this->pose.position[1]);
		position->set_z(this->pose.position[2]);
		auto orientation = pose->mutable_orientation();
		orientation->set_x(this->pose.orientation[0]);
		orientation->set_y(this->pose.orientation[1]);
		orientation->set_z(this->pose.orientation[2]);
		orientation->set_w(this->pose.orientation[3]);

		auto twist_linear = msg.mutable_twist()->mutable_linear();
		twist_linear->set_x(this->twist[0]);
		twist_linear->set_y(this->twist[1]);
		twist_linear->set_z(this->twist[2]);

		std::string msg_data;
		
		if (!msg.SerializeToString(&msg_data)) {

			printf("Image serialize failed\n");
			return 0;
		}

		msg_data = Codex::encode64(msg_data);
		out.write(msg_data.data(), msg_data.length());

		return msg_data.length();
	}

	auto Odometry::parse(std::istream &in) -> unsigned long {

		cmg_pb::Odometry msg;
		
		std::stringstream &ss = dynamic_cast<std::stringstream&>(in);

		std::string msg_data = ss.str();
		msg_data = Codex::decode64(msg_data);

		if (!msg.ParseFromString(msg_data)) {

			printf("Image parse failed\n");
			return 0;
		}

		this->header.stamp.time_ = msg.header().stamp_sec();
		this->header.frame_id = msg.header().frame_id();

		this->pose.position[0] = msg.pose().position().x();
		this->pose.position[1] = msg.pose().position().y();
		this->pose.position[2] = msg.pose().position().z();
		this->pose.orientation[0] = msg.pose().orientation().x();
		this->pose.orientation[1] = msg.pose().orientation().y();
		this->pose.orientation[2] = msg.pose().orientation().z();
		this->pose.orientation[3] = msg.pose().orientation().w();

		this->twist[0] = msg.twist().linear().x();
		this->twist[1] = msg.twist().linear().y();
		this->twist[2] = msg.twist().linear().z();

		return msg_data.length();
	}

}}
