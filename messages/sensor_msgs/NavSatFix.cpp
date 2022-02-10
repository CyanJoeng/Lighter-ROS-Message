/**
 * Author: Cyan
 * Date: Mon Dec 27 17:15:28 CST 2021
 */
#include "NavSatFix.hpp"
#include <sstream>

#include "cmg/message/codex.hpp"
#include "sensor_msgs.pb.h"

namespace cmg { namespace sensor_msgs {

    auto NavSatFix::serialize(std::ostream &out) const -> unsigned long {

        cmg_pb::NavSatFix msg;

        auto header = msg.mutable_header();
        header->set_stamp_sec(this->header.stamp.sec);
        header->set_stamp_nsec(this->header.stamp.nsec);
        header->set_frame_id(this->header.frame_id);

        msg.set_latitude(this->latitude);
        msg.set_longitude(this->longitude);
        msg.set_altitude(this->altitude);

        std::string msg_data;

        if (!msg.SerializeToString(&msg_data)) {

            CMG_WARN("Image serialize failed\n");
            return 0;
        }

        msg_data = Codex::encode64(msg_data);
        out.write(msg_data.data(), msg_data.length());

        return msg_data.length();

    }

    auto NavSatFix::parse(std::istream &in) -> unsigned long {

        cmg_pb::NavSatFix msg;

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

        this->latitude = msg.latitude();
        this->longitude = msg.longitude();
        this->altitude = msg.altitude();

        return msg_data.length();
    }
}}
