/**
 * Author: Cyan
 * Date: Mon Dec  6 15:15:19 CST 2021
 */
#pragma once

#include "node_handle.hpp"
#include <string>
#include <vector>

namespace cmg {

	int init(int argc, char *argv[], const char *proc_name);

	int init_str(const std::vector<std::string> &args, const char *proc_name);

	void spin();
};
