/**
 * @file Array.cpp
 * @author Cyan
 * @brief 
 * @version 0.1
 * @date 2022-04-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "Array.hpp"

#include "std_msgs.pb.h"

namespace cmg { namespace std_msgs {

    auto Array::addMessage(const Message &message) -> std::size_t {

        std::stringstream ss;
        auto msg_len = message.serialize(ss);
        this->msgs_.emplace_back(ss.str());

        return this->msgs_.size();
    }

    auto Array::operator <<(const Message &message) -> Array& {

        if (this->addMessage(message) > 0) {
            
            return *this;
        }

        throw std::logic_error("message is nullptr");
    }

    auto Array::serialize(std::ostream &out) const -> unsigned long {

        cmg_pb::ArrayMsg msg;

        auto _header = msg.mutable_header();
        _header->set_frame_id(this->header.frame_id);
        _header->set_stamp_sec(this->header.stamp.sec);
        _header->set_stamp_nsec(this->header.stamp.nsec);

        for (auto &data : this->msgs_) {

            msg.add_msgs(data);
        }

        if (!msg.SerializeToOstream(&out)) {

            CMG_WARN("Array serialize failed\n");
            return 0;
        }

        return out.tellp();
    }

    auto Array::parse(std::istream &in) -> unsigned long {

        cmg_pb::ArrayMsg array_msg;

        if (!array_msg.ParseFromIstream(&in)) {

            CMG_WARN("Array parse failed\n");
            return 0;
        }

        this->header.frame_id = array_msg.header().frame_id();
        this->header.stamp.sec = array_msg.header().stamp_sec();
        this->header.stamp.nsec = array_msg.header().stamp_nsec();

        for (auto &data : array_msg.msgs()) {

            this->msgs_.emplace_back(data);
        }

        return in.tellg();
    }
}}
