/**
 * Author: Cyan
 * Date: Thu Dec 23 11:35:00 CST 2021
 */
#pragma once

#include <map>
#include <string>

#include <opencv2/core/persistence.hpp>


namespace cmg {

	class Config : private cv::FileStorage {

	public:
		Config(const std::string &file_path);

		template <typename T>
		auto get(const std::string &key, T &val) const -> bool {

			auto root = this->root();
			auto it = std::find_if(root.begin(), root.end(), [&key](auto it) {

					return it.name() == key;
					});
			if (root.end() == it)
				return false;

			val = this->get<T>(key);
			return true;
		}

		template <typename T>
		auto get(const std::string &key) const -> T {

			return (*this)[key];
		}
	};
}
