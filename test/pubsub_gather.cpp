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
#include <opencv2/viz/types.hpp>
#include <opencv2/viz/viz3d.hpp>
#include <opencv2/viz/widgets.hpp>
#include <opencv2/core/eigen.hpp>

#include <cmg/cmg.hpp>

#include <messages/std_msgs/Array.hpp>
#include <messages/nav_msgs/Odometry.hpp>

#include "arg_parse.hpp"

auto draw_gps_anchor(const cmg::nav_msgs::OdometryConstPtr &anchor) -> cv::viz::WSphere {

    auto &pos = anchor->pose.position;
    cv::viz::WSphere anchor_view({pos.x, pos.y, pos.z}, 0.5, 10, cv::viz::Color::white());

    return anchor_view;
}

auto draw_window_frames(const std::vector<cmg::nav_msgs::Odometry> &frame_odos) ->
std::pair<std::vector<cv::viz::WPlane>, cv::viz::WPolyLine> {

    static std::vector<cv::Point3d> hist_points;
    auto &pos = frame_odos.front().pose.position;
    auto &ori = frame_odos.front().pose.orientation;

    hist_points.emplace_back(pos.x, pos.y, pos.z);

    cv::viz::WPolyLine hist_odo (hist_points, cv::viz::Color::orange());

    std::vector<cv::viz::WPlane> win_frame_views;

    for (auto &odo : frame_odos) {

        auto &pos = odo.pose.position;
        auto &ori = odo.pose.orientation;

        Eigen::Vector3d norm = Eigen::Quaterniond(ori.w, ori.x, ori.y, ori.z) * Eigen::Vector3d::UnitZ();
        Eigen::Vector3d newY = Eigen::Quaterniond(ori.w, ori.x, ori.y, ori.z) * Eigen::Vector3d::UnitY();
        cv::viz::WPlane view({pos.x, pos.y, pos.z}, {norm.x(), norm.y(), norm.z()}, {newY.x(), newY.y(), newY.z()}, {0.5, 0.5}, cv::viz::Color::yellow());

        win_frame_views.emplace_back(view);
    }

    return {win_frame_views, hist_odo};
}

auto draw_odo_3d(const cmg::nav_msgs::OdometryConstPtr &odometry) -> std::pair<cv::viz::WPolyLine, cv::Affine3d> {

    static std::vector<cv::Point3d> hist_points;
    auto &pos = odometry->pose.position;
    auto &ori = odometry->pose.orientation;

    hist_points.emplace_back(pos.x, pos.y, pos.z);

    auto polyline = cv::viz::WPolyLine(hist_points);

    cv::Affine3d pose;
    cv::Mat rot;
    Eigen::Matrix3d m = Eigen::Quaterniond(ori.w, ori.x, ori.y, ori.z).matrix();
    cv::eigen2cv(m, rot);

    pose.rotation(rot);
    pose.translation(cv::Vec3d(pos.x, pos.y, pos.z));

    return {polyline, pose};
}


static std::mutex _widget_mt;
static std::condition_variable _widget_cv;
static std::list<std::vector<cv::viz::WPlane>> _widget_window_frames_buf;
static std::list<cv::viz::WPolyLine> _widget_window_frame_odo_buf;
static std::list<cv::viz::WSphere> _widget_anchor_buf;
static std::list<std::pair<cv::viz::WPolyLine, cv::Affine3d>> _widget_odo_buf;

static constexpr auto name_gather = "gather";
static std::map<std::string, std::optional<cv::viz::Viz3d>> _windows;
static std::thread _work_loop;
static bool _loop_exit {false};

void ui_refresh() {

    for (auto &window_name : {"gps_window_frame", "odo_window_frame"}) {

        auto &_window = _windows[window_name];
        _window.emplace(window_name);
        _window->showWidget("Coord", cv::viz::WCoordinateSystem(10.));
    }

    cv::Matx33f K(700, 0, 320, 0, 700, 240, 0, 0);
    auto cam = cv::viz::WCameraPosition(K,  10);

    while (!_loop_exit) {

        std::unique_lock<std::mutex> lck(_widget_mt);
        _widget_cv.wait_for(lck, std::chrono::duration<double>(0.1), []() {
                return _loop_exit || !_widget_window_frames_buf.empty() || !_widget_anchor_buf.empty() || !_widget_odo_buf.empty();
                });

        if (!_loop_exit) {

            if (!_widget_window_frames_buf.empty()) {

                auto window_odo_count = 0;
                for (auto &odo : _widget_window_frames_buf.back()) {

                    std::string widget_name = (boost::format("widget_window_frame_%d") % window_odo_count++).str();
                    _windows["gps_window_frame"]->showWidget(widget_name, odo);
                    _windows["odo_window_frame"]->showWidget(widget_name, odo);
                }
                _widget_window_frames_buf.clear();
            }

            if (!_widget_window_frame_odo_buf.empty()) {

                _windows["gps_window_frame"]->showWidget("widget_frame_odos", _widget_window_frame_odo_buf.back());
                _windows["odo_window_frame"]->showWidget("widget_frame_odos", _widget_window_frame_odo_buf.back());
                _widget_window_frame_odo_buf.clear();
            }

            if (!_widget_anchor_buf.empty()) {

                int idx = _widget_anchor_buf.size();
                _windows["gps_window_frame"]->showWidget("widget_gps_anchor_" + std::to_string(idx), _widget_anchor_buf.back());
            }

            if (!_widget_odo_buf.empty()) {

                auto &[widget_odo, camera_pose] = _widget_odo_buf.back();
                _windows["odo_window_frame"]->showWidget("widget_odo", widget_odo);
                _widget_odo_buf.clear();
            }
        }
        lck.unlock();

        for (auto &[name, _window] : _windows)
            _window->spinOnce();
    }
}

auto cb_gps_anchor(const cmg::nav_msgs::OdometryConstPtr &anchor) -> void {

    auto pos = anchor->pose.position;
    printf("cb gps anchor time:%f (%10.4f, %10.4f, %10.4f)\n", anchor->header.stamp.toSec(),
            pos.x, pos.y, pos.z);

    auto gps_anchor_view = draw_gps_anchor(anchor);
    {
        std::lock_guard<std::mutex> lck(_widget_mt);
        _widget_anchor_buf.emplace_back(std::move(gps_anchor_view));
    }
    _widget_cv.notify_all();
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

    auto [window_frames_view, window_frame_odo_view] = draw_window_frames(window_frame_odos);
    {
        std::lock_guard<std::mutex> lck(_widget_mt);
        _widget_window_frames_buf.emplace_back(std::move(window_frames_view));
        _widget_window_frame_odo_buf.emplace_back(window_frame_odo_view);
    }
    _widget_cv.notify_all();
}

auto cb_odo(const cmg::nav_msgs::OdometryConstPtr &odometry) {

    auto &position = odometry->pose.position;
    printf("cb odometry stamp %f (%10.4f %10.4f %10.4f)\n", odometry->header.stamp.toSec(),
            position.x, position.y, position.z);

    auto [odo, pose] = draw_odo_3d(odometry);
    {
        std::lock_guard<std::mutex> lock(_widget_mt);
        _widget_odo_buf.emplace_back(std::move(odo), pose);
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

    std::string topic_window_frames = "/estimator/window_frames";
    std::string topic_gps_anchor = "/estimator/anc_pose";
    std::string topic_odo = "/estimator/odometry";

    _work_loop = std::thread {[&]() {

        auto sub_window_frames = n.subscribe(topic_window_frames, 1000, cb_window_frames);
        auto sub_gps_anchor = n.subscribe(topic_gps_anchor, 1000, cb_gps_anchor);
        auto sub_odo = n.subscribe(topic_odo, 1000, cb_odo);
        cmg::spin();
    }};
    ui_refresh();

    return 0;
}
