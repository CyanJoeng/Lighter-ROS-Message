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

#include "cmg/utils/log.hpp"


namespace cmg {

	class Config {

		boost::json::object data_;

		std::string root_dir_;

	public:
		Config(const std::string &file_path);

		template <typename T>
		auto get(const std::string &key, T &val) const -> bool {

			try {

				val = this->get<T>(key);
				return true;
			} catch (const boost::exception &e) {
				
				CMG_WARN("[Config](get) can not find item with key %s", key.c_str());
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

			if (key == "root_dir")
				return this->root_dir_;

			return boost::json::value_to<T>(this->data_.at(key));
		}


		template <typename T>
		void set(const std::string &key, const T &val = nullptr) {

			this->data_[key] = val;
		}
	};
}
