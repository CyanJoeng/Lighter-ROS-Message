/**
 * Author: Cyan
 * Date: Wed Dec 22 16:09:59 CST 2021
 */
#include "Odometry.hpp"

#include <sstream>

#include "cmg/message/codex.hpp"
#include "cmg/utils/log.hpp"
#include "nav_msgs.pb.h"

namespace cmg { namespace nav_msgs {

    auto Odometry::serialize(std::ostream &out) const -> unsigned long {

        cmg_pb::Odometry msg;

        auto header = msg.mutable_header();
        header->set_stamp_sec(this->header.stamp.sec);
        header->set_stamp_nsec(this->header.stamp.nsec);
        header->set_frame_id(this->header.frame_id);

        auto pose = msg.mutable_pose();
        auto position = pose->mutable_position();
        position->set_x(this->pose.position.x);
        position->set_y(this->pose.position.y);
        position->set_z(this->pose.position.z);
        auto orientation = pose->mutable_orientation();
        orientation->set_x(this->pose.orientation.x);
        orientation->set_y(this->pose.orientation.y);
        orientation->set_z(this->pose.orientation.z);
        orientation->set_w(this->pose.orientation.w);

        auto twist_linear = msg.mutable_twist()->mutable_linear();
        twist_linear->set_x(this->twist.x);
        twist_linear->set_y(this->twist.y);
        twist_linear->set_z(this->twist.z);

        std::string msg_data;

        if (!msg.SerializeToString(&msg_data)) {

            CMG_WARN("Image serialize failed\n");
            return 0;
        }

        msg_data = Codex::encode64(msg_data);
        out.write(msg_data.data(), msg_data.length());

        return msg_data.length();
    }

    auto Odometry::parse(std::istream &in) -> unsigned long {

        cmg_pb::Odometry msg;

        std::stringstream &ss = dynamic_cast<std::stringstream&>(in);

        std::string msg_data = ss.str();
        msg_data = Codex::decode64(msg_data);

        if (!msg.ParseFromString(msg_data)) {

            CMG_WARN("Image parse failed\n");
            return 0;
        }

        this->header.stamp.sec = msg.header().stamp_sec();
        this->header.stamp.nsec = msg.header().stamp_nsec();
        this->header.frame_id = msg.header().frame_id();

        this->pose.position.x = msg.pose().position().x();
        this->pose.position.y = msg.pose().position().y();
        this->pose.position.z = msg.pose().position().z();
        this->pose.orientation.x = msg.pose().orientation().x();
        this->pose.orientation.y = msg.pose().orientation().y();
        this->pose.orientation.z = msg.pose().orientation().z();
        this->pose.orientation.w = msg.pose().orientation().w();

        this->twist.x = msg.twist().linear().x();
        this->twist.y = msg.twist().linear().y();
        this->twist.z = msg.twist().linear().z();

        return msg_data.length();
    }

}}
