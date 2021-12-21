/**
 * Author: Cyan
 * Date: Tue Dec  7 15:56:32 CST 2021
 */
#include "foo_bar.hpp"
#include <cstdio>

#include <sstream>


#include "protos/foo_bar.pb.h"

namespace cmg { namespace example_msgs {

	FooBarMessage::FooBarMessage(int id, const std::string &extra)
		: id(id), extra(extra) {}

	auto FooBarMessage::serialize(std::ostream &out) -> unsigned long {

		example::FooBar msg;

		msg.set_id(this->id);
		msg.set_extra(this->extra);

		std::string msg_data;
		if (!msg.SerializeToString(&msg_data)) {

			printf("FooBar serialize failed\n");
			return 0;
		}

		out.write(msg_data.data(), msg_data.length());
		return msg_data.length();
	}

	auto FooBarMessage::parse(std::istream &in) -> unsigned long {

		example::FooBar msg;

		std::stringstream &ss = dynamic_cast<std::stringstream&>(in);

		std::string msg_data = ss.str();

		if (!msg.ParsePartialFromString(msg_data)) {

			printf("FooBar parse failed\n");
			return 0;
		}

		this->id = msg.id();
		this->extra = msg.extra();

		return msg.ByteSizeLong();
	}
}}
