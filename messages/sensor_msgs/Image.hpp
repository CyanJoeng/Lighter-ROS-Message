/**
 * Author: Cyan
 * Date: Wed Dec  8 15:41:20 CST 2021
 */
#pragma once

#include <memory>
#include <vector>

#include "cmg/message/message.hpp"
#include "messages/std_msgs/Header.hpp"

namespace cmg { namespace sensor_msgs {

	struct Image : public cmg::Message {

		std_msgs::Header header;

		//cv::Mat image;

		int rows{0};
		int cols{0};

		int channels{0};

		std::vector<char> data;
	
		virtual auto serialize(std::ostream &out) const -> unsigned long final;

		virtual auto parse(std::istream &in) -> unsigned long final;

		auto setData(int rows, int cols, int channels, char *data) -> int;
	};

	using ImagePtr = std::shared_ptr<Image>;
	using ImageConstPtr = std::shared_ptr<const Image>;
}}
