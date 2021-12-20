/**
 * Author: Cyan
 * Date: Tue Dec  7 15:56:32 CST 2021
 */
#include "foo_bar.hpp"
#include <cstdio>


#include "protos/foo_bar.pb.h"

namespace cmg { namespace example_msgs {

	FooBarMessage::FooBarMessage(int id, const std::string &extra)
		: id(id), extra(extra) {}

	auto FooBarMessage::serialize(std::ostream &out) -> unsigned long {

		example::FooBar msg;

		msg.set_id(this->id);
		msg.set_extra(this->extra);

		auto ret = msg.SerializeToOstream(&out);
		if (ret)
			return msg.ByteSizeLong();
		else {

			printf("FooBar serialize failed\n");
		}
		return 0;
	}

	auto FooBarMessage::parse(std::istream &in) -> unsigned long {

		example::FooBar msg;

		if (!msg.ParsePartialFromIstream(&in)) {

			printf("FooBar parse failed\n");
			return 0;
		}

		this->id = msg.id();
		this->extra = msg.extra();

		return msg.ByteSizeLong();
	}
}}
