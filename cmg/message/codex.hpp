/**
 * Author: Cyan
 * Date: Thu Dec 16 15:33:21 CST 2021
 */
#pragma once

#include <string>

namespace cmg {

    struct Codex {

		static auto encode64(const std::string &plaint) -> std::string;

		static auto decode64(const std::string &cipher) -> std::string;
	};

	struct Zip {

		static auto compress(const std::string &plaint) -> std::string;

		static auto decompress(const std::string &compacted) -> std::string;
	};
}
