/**
 * Author: Cyan
 * Date: Wed Dec  8 16:14:11 CST 2021
 */
#include "Image.hpp"
#include <algorithm>
#include <cstring>
#include <sstream>

#include "cmg/message/codex.hpp"
#include "sensor_msgs.pb.h"

namespace cmg { namespace sensor_msgs{

	using namespace google::protobuf;

	auto Image::serialize(std::ostream &out) const -> unsigned long {

#if 0
		out.write((char*)&this->rows, sizeof(this->rows));
		out.write((char*)&this->cols, sizeof(this->cols));
		out.write((char*)&this->channels, sizeof(this->channels));
		out.write((char*)this->data.data(), this->data.size());

		return out.tellp();
#endif

		cmg_pb::Image msg;

		auto header = msg.mutable_header();
		header->set_stamp_sec(this->header.stamp.toSec());
		header->set_frame_id(this->header.frame_id);

		msg.set_height(this->rows);
		msg.set_width(this->cols);
		msg.set_channels(this->channels);
		auto len = this->rows * this->cols * this->channels;
		msg.set_data(this->data.data(), len);

		std::string msg_data;
		
		if (!msg.SerializeToString(&msg_data)) {

			CMG_WARN("Image serialize failed\n");
			return 0;
		}

		//msg_data = Codex::encode64(msg_data);
		msg_data = Zip::compress(msg_data);
		out.write(msg_data.data(), msg_data.length());

		return msg_data.length();
	}

	auto Image::parse(std::istream &in) -> unsigned long {

#if 0
		in.read((char*)&this->rows, sizeof(this->rows));
		in.read((char*)&this->cols, sizeof(this->cols));
		in.read((char*)&this->channels, sizeof(this->channels));

		this->data.resize(this->rows * this->cols * this->channels);
		in.read((char*)this->data.data(), this->data.size());

		return in.tellg();
#endif

		cmg_pb::Image msg;
		
		std::stringstream &ss = dynamic_cast<std::stringstream&>(in);

		std::string msg_data = ss.str();
		msg_data = Zip::decompress(msg_data);
		//msg_data = Codex::decode64(msg_data);

		if (!msg.ParseFromString(msg_data)) {

			CMG_WARN("Image parse failed\n");
			return 0;
		}

		this->header.stamp.time_ = msg.header().stamp_sec();
		this->header.frame_id = msg.header().frame_id();

		this->rows = msg.height();
		this->cols = msg.width();
		this->channels = msg.channels();
		auto *data = msg.mutable_data()->data();
		this->data.resize(this->rows * this->cols * this->channels);
		memcpy(this->data.data(), data, this->data.size());

		return msg_data.length();
	}

	auto Image::setData(int rows, int cols, int channels, char *data) -> int {

		this->rows = rows;
		this->cols = cols;
		this->channels = channels;

		this->data.resize(this->rows * this->cols * this->channels);
		std::copy(data, data + this->data.size(), this->data.begin());

		return this->data.size();
	}
}}
