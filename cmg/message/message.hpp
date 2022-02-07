/**
 * Author: Cyan
 * Date: Thu Dec  2 16:44:16 CST 2021
 */ 
#pragma once
#include <cstdio>
#include <ios>
#include <iostream>
#include <string>

#include "cmg/utils/log.hpp"

namespace cmg {

	class Message {

	protected:
		virtual ~Message() {}

	public:
		virtual auto serialize(std::ostream &out) const -> unsigned long = 0;

		virtual auto parse(std::istream &in) -> unsigned long = 0;
	};


	class StrMessage : public Message {

	public:
		std::string data_;

	public:
		StrMessage() = default;

		StrMessage(const std::string &str)
			: data_(str) {}

		~StrMessage() {}

		virtual auto serialize(std::ostream &out) const -> unsigned long final {

			out.write(this->data_.data(), this->data_.length());
			return this->data_.length();
		}

		virtual auto parse(std::istream &in) -> unsigned long final {

			in.seekg(0, std::ios::end);
			long len = in.tellg();
			in.seekg(0);

			in.read((char*)this->data_.data(), len);

			if (in.gcount() != len) {

				CMG_WARN("parse from in stream failed (%ld <- %td)\n", len, in.gcount());
			}

			return len;
		}
	};
}
