/**
 * Author: Cyan
 * Date: Wed Dec  8 16:14:11 CST 2021
 */
#include "Image.hpp"
#include "protos/sensor_msgs.pb.h"
#include <algorithm>
#include <cstring>

namespace cmg { namespace sensor_msgs{

	auto Image::serialize(std::ostream &out) -> unsigned long {

		cmg_pb::Image msg;

		auto header = msg.mutable_header();
		header->set_stamp_sec(this->header.stamp.toSec());
		header->set_frame_id(this->header.frame_id);

		msg.set_height(this->rows);
		msg.set_width(this->cols);
		msg.set_type(this->type);
		auto len = this->rows * this->cols * 3;
		msg.set_data(this->data.data(), len);

		if (msg.SerializePartialToOstream(&out))
			return msg.ByteSizeLong();
		else
			printf("Image serialize failed\n");
		return 0;
	}

	auto Image::parse(std::istream &in) -> unsigned long {

		cmg_pb::Image msg;

		if (!msg.ParseFromIstream(&in)) {

			printf("Image parse failed\n");
			return 0;
		}

		this->header.stamp.time_ = msg.header().stamp_sec();
		this->header.frame_id = msg.header().frame_id();

		this->rows = msg.height();
		this->cols = msg.width();
		this->type = msg.type();
		auto *data = msg.mutable_data()->data();
		this->data.resize(this->rows * this->cols * 3);
		memcpy(this->data.data(), data, this->data.size());

		return msg.ByteSizeLong();
	}

	auto Image::setData(int rows, int cols, char *data) -> int {

		this->rows = rows;
		this->cols = cols;

		this->data.resize(this->rows * this->cols * 3);
		std::copy(data, data + this->data.size(), this->data.begin());

		return this->data.size();
	}
}}
