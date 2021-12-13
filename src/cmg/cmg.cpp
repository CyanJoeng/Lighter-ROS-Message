/**
 * Author: Cyan
 * Date: Mon Dec  6 15:22:58 CST 2021
 */
#include "cmg.hpp"

#include <fstream>

#include "cmg/socket.hpp"


bool init_prococess_port(const std::string& cfg_file_path) {

	std::ifstream in(cfg_file_path);

	if (!in.is_open()) {

		printf("cfg file can not open: %s\n", cfg_file_path.c_str());
		return false;
	}

	std::string proc_name;
	while (true) {

		in >> proc_name;
		if (!proc_name.empty())
			break;
		
		auto offset = cmg::URL::proc_ports.size();
		cmg::URL::proc_ports[proc_name] = cmg::URL::BASE_PORT + offset;
	}

	return true;
}


int cmg::init(int argc, char **argv, const char *proc_name) {

	Enviroment::Inst(proc_name);

	return 0;
}

void cmg::spin() {

	Enviroment::Spin();
}
