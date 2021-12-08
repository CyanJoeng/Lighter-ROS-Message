/**
 * Author: Cyan
 * Date: Wed Dec  8 15:41:20 CST 2021
 */
#pragma once

#include <opencv2/core/mat.hpp>

#include "messages/message.hpp"
#include "messages/std_msgs/Header.hpp"

namespace sensor_msgs {

	struct Image : public cmg::Message {

		std_msgs::Header header;

		cv::Mat image;
		
		virtual auto serialize(std::ostream &out) -> unsigned long final;

		virtual auto parse(std::istream &in) -> unsigned long final;
	};

	using ImagePtr = std::shared_ptr<Image>;
}
