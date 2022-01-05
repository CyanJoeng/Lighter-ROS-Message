/**
 * Author: Cyan
 * Date: Thu Dec 23 11:35:00 CST 2021
 */
#pragma once

#include <map>
#include <any>
#include <string>


namespace cmg {

	class Config : private std::map<std::string, std::any> {

	public:
		Config(const std::string &file_path);

		template <typename T>
		auto get(const std::string &key, T &val) const -> bool {

			auto it = this->find(key);
			if (this->end() == it)
				return false;

			val = this->get<T>(key);
			return true;
		}

		template <typename T>
		auto get(const std::string &key) const -> T {

			auto val = this->at(key);
		}
	};
}
