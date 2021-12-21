/**
 * Author: Cyan
 * Date: Mon Dec  6 15:22:58 CST 2021
 */
#include "cmg.hpp"

#include <fstream>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <tuple>

#include "cmg/socket.hpp"


bool init_process_port(const std::string& cfg_file_path) {

	std::ifstream in(cfg_file_path);

	struct _IT {

		std::string proc;
		std::string ip;
		std::vector<std::string> topics;
	};

	if (!in.is_open()) {

		std::list<_IT> cfg = {

			{"station", "172.18.135.103", {
											  "match",
											  "foo"
										  }},
			{"client", "0.0.0.0", {}}
		};

		for (auto [proc_name, ip, topics] : cfg)
			for (auto &topic : topics)
				cmg::URL::RegistProc(proc_name, topic, ip);


		return true;
	}

	std::string tag;
	std::string proc_name;
	std::string ip;
	std::string topic;

	while (!in.eof()) {

		in >> tag;
		if (in.eof())
			break;

		if (tag == "-")
			in >> proc_name >> ip;
		else if (tag == "=") {

			in >> topic;
			//printf("[CMG][config] proc %s\t topic %s\t ip %s\n", proc_name.c_str(), topic.c_str(), ip.c_str());
			cmg::URL::RegistProc(proc_name, topic, ip);
		}
	}

	return true;
}

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

	{
		std::lock_guard<std::mutex> lck(init_mt);
		if (inited) return -1;

		init_process_port(args[1]);
		inited = true;
	}

	Environment::Inst(proc_name);

	return 0;
}

void cmg::spin() {

	Environment::Spin();
}
