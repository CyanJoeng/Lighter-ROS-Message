/**
 * Author: Cyan
 * Date: Thu Dec 23 18:29:12 CST 2021
 */
#pragma once

#include "messages/message.hpp"
namespace cmg { namespace std_msgs {

	struct Bool : Message {

		static constexpr auto Len = sizeof(bool);

		bool data {false};

		virtual auto serialize(std::ostream &out) const -> unsigned long {

			out.write(reinterpret_cast<const char *>(&this->data), Len);
			return Len;
		}

		virtual auto parse(std::istream &in) -> unsigned long {

			in.read(reinterpret_cast<char *>(&this->data), Len);
			return Len;
		}
	};

	using BoolConstPtr = std::shared_ptr<const Bool>;
}}
