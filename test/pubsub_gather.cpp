/**
 * Author: Cyan
 * Thu Apr  7 19:55:51 CST 2022
 */
#include <list>
#include <mutex>
#include <cstdio>
#include <condition_variable>

#include <boost/range/irange.hpp>
#include <boost/format.hpp>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <opencv2/core/types.hpp>
#include <opencv2/viz/viz3d.hpp>
#include <opencv2/viz/widgets.hpp>

#include <cmg/cmg.hpp>

#include <messages/std_msgs/Array.hpp>
#include <messages/nav_msgs/Odometry.hpp>

#include "arg_parse.hpp"

auto draw_window_frames(const std::vector<cmg::nav_msgs::Odometry> &frame_odos) -> std::vector<cv::viz::WPlane> {

    std::vector<cv::viz::WPlane> win_frame_views(frame_odos.size());

    for (auto &odo : frame_odos) {

        auto &pos = odo.pose.position;
        auto &ori = odo.pose.orientation;

        Eigen::Vector3d norm = Eigen::Quaterniond(ori.w, ori.x, ori.y, ori.z) * Eigen::Vector3d::UnitZ();
        Eigen::Vector3d newY = Eigen::Quaterniond(ori.w, ori.x, ori.y, ori.z) * Eigen::Vector3d::UnitY();
        cv::viz::WPlane view(cv::Point3d(pos.x, pos.y, pos.z), cv::Vec3d(norm.x(), norm.y(), norm.z()), cv::Vec3d(newY.x(), newY.y(), newY.z()));

        win_frame_views.emplace_back(view);
    }

    return win_frame_views;
}

static std::mutex _widget_mt;
static std::condition_variable _widget_cv;
static std::list<std::vector<cv::viz::WPlane>> _widget_buf;

static constexpr auto name_gather = "gather";
static std::optional<cv::viz::Viz3d> _window;
static std::thread _work_loop;
static bool _loop_exit {false};

void ui_refresh() {

	_window.emplace(name_gather);
	_window->showWidget("Coord", cv::viz::WCoordinateSystem(10.));

	cv::Matx33f K(700, 0, 320, 0, 700, 240, 0, 0);
	auto cam = cv::viz::WCameraPosition(K,  10);

	while (!_loop_exit) {

        std::unique_lock<std::mutex> lck(_widget_mt);
        _widget_cv.wait(lck, []() {
                return _loop_exit || !_widget_buf.empty();
                });

        if (!_loop_exit) {

            if (!_widget_buf.empty()) {

                static auto window_odo_count = 0;
                for (auto idx : boost::irange(window_odo_count)) {

                    std::string widget_name = (boost::format("window_frame_odo_%d") % idx).str();
                    _window->removeWidget(widget_name);
                }

                window_odo_count = 0;

                for (auto &odo : _widget_buf.back()) {

                    std::string widget_name = (boost::format("window_frame_odo_%d") % window_odo_count++).str();
                    _window->showWidget(widget_name, odo);
                }
            }
        }
        lck.unlock();

		_window->spinOnce();
	}
}

auto cb_window_frames(const cmg::std_msgs::ArrayConstPtr &frames) -> void {

    std::vector<cmg::nav_msgs::Odometry> window_frame_odos(frames->size());
    for (auto idx : boost::irange(frames->size())) {

        window_frame_odos[idx] = frames->item<cmg::nav_msgs::Odometry>(idx);
    }

    auto front_pos = window_frame_odos.back().pose.position;
    printf("cb window frames time:%f sz:%zu (%10.4f, %10.4f, %10.4f)\n",
            frames->header.stamp.toSec(), frames->size(),
            front_pos.x, front_pos.y, front_pos.z);

    auto window_frames_view = draw_window_frames(window_frame_odos);
    {
        std::lock_guard<std::mutex> lck(_widget_mt);
        _widget_buf.emplace_back(std::move(window_frames_view));
    }
    _widget_cv.notify_all();
}


int main(int argc, char *argv[]) {

	std::string client_proc_name = "client";

	auto args = args_parser(argc, argv);

	char mode = args["mode"].as<char>();
	std::string proc = args["proc"].as<std::string>();
	std::string topic = args["topic"].as<std::string>();
	std::string cfg = args["cfg"].as<std::string>();

	const char *proc_args[] = {
		argv[0],
		cfg.data()
	};

    cmg::init(2, proc_args, client_proc_name.c_str());

    cmg::NodeHandle n("~");

    std::string proc_topic = "/" + proc + "/" + topic;

    _work_loop = std::thread {[&]() {

        auto sub = n.subscribe(proc_topic, 1000, cb_window_frames);
        cmg::spin();
    }};
    ui_refresh();

	return 0;
}
