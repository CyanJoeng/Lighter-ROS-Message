/**
 * Author: Cyan
 * Date: Mon Dec  6 15:15:19 CST 2021
 */
#pragma once
#include <string>
#include <vector>

#include "core/node_handle.hpp"
#include "utils/log.hpp"
#include "utils/time.hpp"

namespace cmg {

	int init(int argc, const char * const argv[], const char *proc_name);

	int init_str(const std::vector<std::string> &args, const char *proc_name);

	void spin();

	void exit();
}
