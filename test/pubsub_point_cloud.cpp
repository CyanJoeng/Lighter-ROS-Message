/**
 * Author: Cyan
 * Date: Tue Dec  7 23:30:10 CST 2021
 */

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <memory>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <opencv2/core/eigen.hpp>
#include <opencv2/viz.hpp>

#include "cmg/cmg.hpp"
#include "messages/sensor_msgs/PointCloud.hpp"

#include "arg_parse.hpp"


using namespace cmg;

using Widget = cv::viz::WCloud;
using WidgetPtr = std::shared_ptr<Widget>;

static std::string name_pts = "point_cloud";


auto create_point_cloud() -> sensor_msgs::PointCloud {

    sensor_msgs::PointCloud feature_points;

    sensor_msgs::ChannelFloat32 id_of_point;
    sensor_msgs::ChannelFloat32 u_of_point;
    sensor_msgs::ChannelFloat32 v_of_point;
    sensor_msgs::ChannelFloat32 velocity_x_of_point;
    sensor_msgs::ChannelFloat32 velocity_y_of_point;

    //feature_points.header;  // img_msg->header;
    feature_points.header.frame_id = "world";
    feature_points.header.stamp = (rand() % 1000 * 1e-3);

    for (auto i = rand() % 1000; i < 1000; ++i) {

        geometry_msgs::Point32 p;
        p.x = i * 0.01;
        p.y = i * 0.02;
        p.z = 1;

        feature_points.points.push_back(p);

        id_of_point.values.push_back(i);
        u_of_point.values.push_back(i * 10);
        v_of_point.values.push_back(i * 11);
        velocity_x_of_point.values.push_back(i * 1.0);
        velocity_y_of_point.values.push_back(i * 1.1);
    }

    feature_points.channels.push_back(id_of_point);
    feature_points.channels.push_back(u_of_point);
    feature_points.channels.push_back(v_of_point);
    feature_points.channels.push_back(velocity_x_of_point);
    feature_points.channels.push_back(velocity_y_of_point);

    return feature_points;
}

auto draw_point_cloud(const cmg::sensor_msgs::PointCloudConstPtr &cloud) -> std::pair<std::vector<WidgetPtr>, cv::Affine3d> {

	static std::vector<cv::Point3d> hist_pts(1);

	std::vector<cv::Point3d> pts;
    pts.resize(cloud->points.size() + 1);

    for (auto i = 0; i < cloud->points.size(); ++i) {

        auto &pt = pts[i];
        pt.x = cloud->points[i].x;
        pt.y = cloud->points[i].y;
        pt.z = cloud->points[i].z;
    }

	auto cloud_wid = std::make_shared<Widget>(pts, cv::viz::Color::red());
    auto hist_cloud_wid = std::make_shared<Widget>(hist_pts, cv::viz::Color::gray());

    hist_pts.insert(hist_pts.end(), pts.begin(), pts.end());

    cv::Affine3d pose;
    if (cloud->channels.size() >= 2) {

        auto &pos = cloud->channels[0].values;
        auto &ori = cloud->channels[1].values;

        cv::Mat rot;
        Eigen::Matrix3d m = Eigen::Quaterniond(ori[3], ori[0], ori[1], ori[2]).matrix();
        cv::eigen2cv(m, rot);

        pose.rotation(rot);
        pose.translation(cv::Vec3d(pos[0], pos[1], pos[2]));
    }

	return {{cloud_wid, hist_cloud_wid}, pose};
}

static std::optional<cv::viz::Viz3d> window;
static std::vector<WidgetPtr> widget_pts;
static cv::Affine3d cam_pose;

static std::mutex widget_mt;
static std::condition_variable widget_cv;
static std::thread work_loop;

void ui_refresh() {

	window.emplace(name_pts);
	window->showWidget("Coord", cv::viz::WCoordinateSystem(10.));

	cv::Matx33f K(700, 0, 320, 0, 700, 240, 0, 0);
	auto cam = cv::viz::WCameraPosition(K,  1);

	while (true) {

		{
			std::unique_lock<std::mutex> lck(widget_mt);
			widget_cv.wait(lck, []() {
					return !widget_pts.empty();
					});

			window->showWidget(name_pts, *widget_pts.front());
			window->showWidget(name_pts + "_hist", *widget_pts.back());
			window->showWidget("Cam", cam, cam_pose);

			lck.unlock();
		}

		window->spinOnce();
	}
}

auto cb(const std::shared_ptr<const sensor_msgs::PointCloud> &point_cloud) {

    printf("cb point_cloud count %zu stamp %f\n", point_cloud->points.size(), point_cloud->header.stamp.toSec());

    auto [pts, pose] = draw_point_cloud(point_cloud);
	{
		std::lock_guard<std::mutex> lock(widget_mt);
		widget_pts.swap(pts);
		cam_pose = pose;
	}
	widget_cv.notify_all();
}

int main(int argc, char *argv[]) {

    const auto server_proc_name = "server";
    const auto client_proc_name = "client";


	auto args = args_parser(argc, argv);

    char mode = args["mode"].as<char>();
	std::string proc = args["proc"].as<std::string>();
	std::string topic = args["topic"].as<std::string>();
	std::string cfg = args["cfg"].as<std::string>();

	const char *proc_args[] = {
		argv[0],
		cfg.data()
	};

    if (mode == 's') {

		cmg::init(2, proc_args, server_proc_name);

        cmg::NodeHandle n("~");

        auto pub_point_cloud = n.advertise<sensor_msgs::PointCloud>("foo", 1000);

        for (auto i = 0; i < 10;++i) {

            auto msg_point_cloud = create_point_cloud();
            pub_point_cloud.publish(msg_point_cloud);

            std::this_thread::sleep_for(std::chrono::duration<double>(1.0));
        }

        cmg::spin();

    } else if (mode == 'c') {

		cmg::init(2, proc_args, client_proc_name);

        cmg::NodeHandle n("~");

		std::string proc_topic = "/" + proc + "/" + topic;

        work_loop = std::thread {[&]() {

            auto sub = n.subscribe(proc_topic, 1000, cb);
			cmg::spin();
		}};
		ui_refresh();
    }

    return 0;
}
