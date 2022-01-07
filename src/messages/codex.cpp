/**
 * Author: Cyan
 * Date: Thu Dec 16 15:34:23 CST 2021
 */
#include "codex.hpp"

#include <string>
#include <sstream>

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>


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


	auto Zip::compress(const std::string &in_str) -> std::string {

		namespace bio = boost::iostreams;

		std::stringstream compressed;
		std::stringstream origin(in_str);

		bio::filtering_streambuf<bio::input> out;
		out.push(bio::zlib_compressor(bio::zlib_params(bio::zlib::best_compression)));
		out.push(origin);
		bio::copy(out, compressed);

		return compressed.str();
	}

	auto Zip::decompress(const std::string &in_str) -> std::string {

		namespace bio = boost::iostreams;

		std::stringstream compressed(in_str);
		std::stringstream decompressed;

		bio::filtering_streambuf<bio::input> out;
		out.push(bio::zlib_decompressor());
		out.push(compressed);
		bio::copy(out, decompressed);

		return decompressed.str();
	}
}
