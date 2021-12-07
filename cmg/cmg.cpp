/**
 * Author: Cyan
 * Date: Mon Dec  6 15:22:58 CST 2021
 */
#include "cmg.hpp"


int cmg::init(int argc, char **argv, const char *proc_name) {

	Enviroment::Inst(proc_name);

	return 0;
}

void cmg::spin() {

	Enviroment::Spin();
}
