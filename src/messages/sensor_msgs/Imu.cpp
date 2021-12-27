/**
 * Author: Cyan
 * Date: Mon Dec 27 16:31:48 CST 2021
 */
#include "Imu.hpp"

#include <sstream>

#include "messages/codex.hpp"
#include "protos/sensor_msgs.pb.h"

namespace cmg { namespace sensor_msgs {

	auto Imu::serialize(std::ostream &out) const -> unsigned long {

		cmg_pb::Imu msg;

		auto header = msg.mutable_header();
		header->set_stamp_sec(this->header.stamp.toSec());
		header->set_frame_id(this->header.frame_id);

		auto _linear = msg.mutable_linear_acceleration();
		_linear->set_x(this->linear_acceleration.x);
		_linear->set_y(this->linear_acceleration.y);
		_linear->set_z(this->linear_acceleration.z);

		auto _angular = msg.mutable_angular_velocity();
		_angular->set_x(this->angular_velocity.x);
		_angular->set_y(this->angular_velocity.y);
		_angular->set_z(this->angular_velocity.z);

		std::string msg_data;
		
		if (!msg.SerializeToString(&msg_data)) {

			printf("Image serialize failed\n");
			return 0;
		}

		msg_data = Codex::encode64(msg_data);
		out.write(msg_data.data(), msg_data.length());

		return msg_data.length();
	}

	auto Imu::parse(std::istream &in) -> unsigned long {

		cmg_pb::Imu msg;
		
		std::stringstream &ss = dynamic_cast<std::stringstream&>(in);

		std::string msg_data = ss.str();
		msg_data = Codex::decode64(msg_data);

		if (!msg.ParseFromString(msg_data)) {

			printf("Image parse failed\n");
			return 0;
		}

		this->header.stamp.time_ = msg.header().stamp_sec();
		this->header.frame_id = msg.header().frame_id();

		this->linear_acceleration.x = msg.linear_acceleration().x();
		this->linear_acceleration.y = msg.linear_acceleration().y();
		this->linear_acceleration.z = msg.linear_acceleration().z();
		this->angular_velocity.x = msg.angular_velocity().x();
		this->angular_velocity.y = msg.angular_velocity().y();
		this->angular_velocity.z = msg.angular_velocity().z();

		return msg_data.length();
	}
}}
