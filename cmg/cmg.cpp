/**
 * Author: Cyan
 * Date: Mon Dec  6 15:22:58 CST 2021
 */
#include "cmg.hpp"

#include <cstdio>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <tuple>

#include "core/socket.hpp"


static bool inited = false;
static std::mutex init_mt;

int cmg::init(int argc, const char * const argv[], const char *proc_name) {

	std::vector<std::string> args;
	args.reserve(argc);
	for (auto i = 0; i < argc; ++i)
		args.emplace_back(argv[i]);

	return cmg::init_str(args, proc_name);
}

int cmg::init_str(const std::vector<std::string> &args, const char *proc_name) {

	if (args.size() < 2) {

		printf("[cmg]init with 3 args(%zu): %s config_cmg.json\n", args.size(), args.front().c_str());
		return -1;
	}

	{
		std::lock_guard<std::mutex> lck(init_mt);
		if (inited) return -1;

		Environment::Config(args[1]);
		inited = true;
	}

	Environment::Inst(proc_name);

	return 0;
}

void cmg::spin() {

	Environment::Spin();
}
