/**
 * Author: Cyan
 * Date: Tue Dec  7 15:56:32 CST 2021
 */
#include "foo_bar.hpp"
#include <cstdio>

namespace example_msgs {

	FooBarMessage::FooBarMessage(int id, const std::string &extra) {

		this->msg.set_id(id);
		this->msg.set_extra(extra);
	}

	auto FooBarMessage::serialize(std::ostream &out) -> unsigned long {

		auto ret = this->msg.SerializeToOstream(&out);
		if (ret)
			return this->msg.ByteSizeLong();
		else {

			printf("FooBar serialize failed\n");
		}
		return 0;
	}

	auto FooBarMessage::parse(std::istream &in) -> unsigned long {

		if (this->msg.ParsePartialFromIstream(&in))
			return this->msg.ByteSizeLong();
		else
			printf("FooBar parse failed\n");
		return 0;
	}
}
