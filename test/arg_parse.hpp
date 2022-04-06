#pragma once
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

auto args_parser(int argc, char *argv[]) -> po::variables_map {

	po::options_description desc("Socket connection test demo");
	desc.add_options()
		("help", "print this message")
		("mode", po::value<char>()->required(), "socker mode")
		("proc", po::value<std::string>()->default_value("server"), "server proc name")
		("topic", po::value<std::string>()->default_value("foo"), "topic name")
		("cfg", po::value<std::string>()->default_value(""), "config file")
		("bg_image", po::value<std::string>()->default_value(""), "background image of canvas")
		("gl", po::value<bool>()->default_value(true), "enable opengl window");

	po::positional_options_description pos_desc;
	pos_desc.add("mode", 1);
		
	po::command_line_parser parser = po::command_line_parser(argc, argv).options(desc).positional(pos_desc);

	po::variables_map vm;
	po::store(parser.run(), vm);
	po::notify(vm);

	if (vm.count("help") || !vm.count("mode")) {

		std::cout << "Usage: " << argv[0] << " ";
		for (auto i = 0; i < pos_desc.max_total_count(); ++i)
			std::cout << pos_desc.name_for_position(i) << " ";
		std::cout << "[options]" << std::endl;
		
		std::cout << desc << std::endl;
		exit(0);
	}

	return vm;
}
