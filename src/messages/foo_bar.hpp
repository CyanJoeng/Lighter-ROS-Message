/**
 * Author: Cyan
 * Date: Tue Dec  7 15:52:47 CST 2021
 */
#pragma  once
#include "message.hpp"

#include "protos/foo_bar.pb.h"

namespace cmg { namespace example_msgs {

	class FooBarMessage : public cmg::Message {

	public:
		example::FooBar msg;

	public:
		FooBarMessage(int id = 0, const std::string &extra = "");

		virtual auto serialize(std::ostream &out) -> unsigned long final;

		virtual auto parse(std::istream &in) -> unsigned long final; 
	};
}}
