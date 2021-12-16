/**
 * Author: Cyan
 * Date: Thu Dec 16 15:34:23 CST 2021
 */
#include "codex.hpp"

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

namespace cmg {

	using namespace boost::archive::iterators;

	auto Codex::encode64(const std::string &plaint) -> std::string {

		using It = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;

		auto tmp = std::string(It(std::begin(plaint)), It(std::end(plaint)));
		return tmp.append((3 - plaint.size() % 3) % 3, '=');
	}

	auto Codex::decode64(const std::string &cipher) -> std::string {

		using It = transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;

		auto tmp = std::string(It(std::begin(cipher)), It(std::end(cipher)));
		return boost::algorithm::trim_right_copy_if(tmp, [](char c) {
				return c == '\0';
				});
	}
}
