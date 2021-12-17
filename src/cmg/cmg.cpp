/**
 * Author: Cyan
 * Date: Mon Dec  6 15:22:58 CST 2021
 */
#include "cmg.hpp"

#include <fstream>
#include <mutex>
#include <vector>

#include "cmg/socket.hpp"


bool init_prococess_port(const std::string& cfg_file_path) {

	std::ifstream in(cfg_file_path);

	if (!in.is_open()) {

		printf("cfg file can not open: %s\n", cfg_file_path.c_str());
		return false;
	}

	std::string proc_name;
	std::string ip;

	while (!in.eof()) {

		in >> proc_name >> ip;
		printf("[CMG][config] proc %s\t ip %s\n", proc_name.c_str(), ip.c_str());
		if (proc_name.empty())
			break;
		
		cmg::URL::RegistProc(proc_name, ip);
	}

	return true;
}

static bool inited = false;
static std::mutex init_mt;

int cmg::init(int argc, char **argv, const char *proc_name) {

	std::vector<std::string> args;
	for (auto i = 0; i < argc; ++i)
		args.emplace_back(argv[i]);

	return cmg::init_str(args, proc_name);
}

int cmg::init_str(const std::vector<std::string> &args, const char *proc_name) {

	{
		std::lock_guard<std::mutex> lck(init_mt);
		if (inited) return -1;

		init_prococess_port(args[1]);
		inited = true;
	}

	Environment::Inst(proc_name);

	return 0;
}

void cmg::spin() {

	Environment::Spin();
}
