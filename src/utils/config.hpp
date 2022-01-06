/**
 * Author: Cyan
 * Date: Thu Dec 23 11:35:00 CST 2021
 */
#pragma once

#include <map>
#include <any>
#include <vector>
#include <string>

#include <boost/json.hpp>


namespace cmg {

	class Config {

		boost::json::value data_;

	public:
		Config(const std::string &file_path);

		template <typename T>
		auto get(const std::string &key, T &val) const -> bool {

			try {

				auto it = this->data_.at(key);
				val = boost::json::value_to<T>(it);
				return true;
			} catch (const boost::exception &e) {
				
				printf("[Config](get) can not find item with key %s\n", key.c_str());
				return false;
			}
		}

		template <typename T>
		auto get(const std::vector<std::string> &keys) const -> T {

			boost::json::value it = this->data_;
			for (auto &key : keys)
				it = it.at(key);

			return boost::json::value_to<T>(it);
		}

		template <typename T>
		auto get(const std::string &key) const -> T {

			return boost::json::value_to<T>(this->data_.at(key));
		}
	};
}
