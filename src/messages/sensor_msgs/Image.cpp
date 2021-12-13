/**
 * Author: Cyan
 * Date: Wed Dec  8 16:14:11 CST 2021
 */
#include "Image.hpp"
#include "protos/sensor_msgs.pb.h"

namespace cmg { namespace sensor_msgs{

	auto Image::serialize(std::ostream &out) -> unsigned long {

		cmg_pb::Image msg;

		auto header = msg.mutable_header();
		header->set_stamp_sec(this->header.stamp.toSec());
		header->set_frame_id(this->header.frame_id);

		msg.set_height(this->image.rows);
		msg.set_width(this->image.cols);
		msg.set_type(this->image.type());
		auto len = this->image.size().area() * this->image.elemSize();
		msg.set_data(this->image.data, len);

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

		int rows = msg.height();
		int cols = msg.width();
		int type = msg.type();
		auto *data = msg.mutable_data()->data();

		this->image = cv::Mat(rows, cols, type, (void*)data).clone();

		return msg.ByteSizeLong();
	}
}}
