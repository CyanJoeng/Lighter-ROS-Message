/**
 * Author: Cyan
 * Date: Mon Dec 27 17:36:49 CST 2021
 */
#include "StampedFloat64Array.hpp"
#include <sstream>

#include "cmg/message/codex.hpp"
#include "std_msgs.pb.h"

namespace cmg {

	auto StampedFloat64Array::serialize(std::ostream &out) const -> unsigned long {

		cmg_pb::StampedFloat64Array msg;

		auto header = msg.mutable_header();
		header->set_stamp_sec(this->header.stamp.toSec());
		header->set_frame_id(this->header.frame_id);

		auto data_ = msg.mutable_data();
		data_->Add(this->data.begin(), this->data.end());

		std::string msg_data;
		
		if (!msg.SerializeToString(&msg_data)) {

			CMG_WARN("Image serialize failed\n");
			return 0;
		}

		msg_data = Codex::encode64(msg_data);
		out.write(msg_data.data(), msg_data.length());

		return msg_data.length();
	}

	auto StampedFloat64Array::parse(std::istream &in) -> unsigned long {

		cmg_pb::StampedFloat64Array msg;
		
		std::stringstream &ss = dynamic_cast<std::stringstream&>(in);

		std::string msg_data = ss.str();
		msg_data = Codex::decode64(msg_data);

		if (!msg.ParseFromString(msg_data)) {

			CMG_WARN("Image parse failed\n");
			return 0;
		}

		this->header.stamp.time_ = msg.header().stamp_sec();
		this->header.frame_id = msg.header().frame_id();

		this->data.resize(msg.data_size());
		std::copy(msg.data().begin(), msg.data().end(), this->data.begin());

		return msg_data.length();
	}
}
