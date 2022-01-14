/**
 * Author: Cyan
 * Date: Tue Dec  7 15:52:47 CST 2021
 */
#pragma  once
#include <string>

#include "cmg/message/message.hpp"


namespace cmg { namespace example_msgs {

	class FooBarMessage : public cmg::Message {

	public:
		unsigned id {0};
		double timestamp {0};
		std::string extra {""};

	public:
		FooBarMessage(int id = 0, double timestamp = 0., const std::string &extra = "");

		virtual auto serialize(std::ostream &out) const -> unsigned long final;

		virtual auto parse(std::istream &in) -> unsigned long final; 
	};
}}
