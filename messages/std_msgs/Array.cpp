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

        auto _data_list = msg.mutable_msg_list();

        auto _header = msg.mutable_header();
        _header->set_frame_id(this->header.frame_id);
        _header->set_stamp_sec(this->header.stamp.sec);
        _header->set_stamp_nsec(this->header.stamp.nsec);

        for (auto &data : this->msgs_) {

            _data_list->Add(data.begin(), data.end());
        }

        std::string msg_data;

        if (!msg.SerializeToString(&msg_data)) {

            CMG_WARN("Image serialize failed\n");
            return 0;
        }

        //msg_data = Codex::encode64(msg_data);
        out.write(msg_data.data(), msg_data.length());

        return msg_data.length();
    }

    auto Array::parse(std::istream &in) -> unsigned long {

        cmg_pb::ArrayMsg array_msg;

        std::stringstream &ss = dynamic_cast<std::stringstream&>(in);

        std::string msg_data = ss.str();
        //msg_data = Codex::decode64(msg_data);

        if (!array_msg.ParseFromString(msg_data)) {

            CMG_WARN("Image parse failed\n");
            return 0;
        }

        this->header.frame_id = array_msg.header().frame_id();
        this->header.stamp.sec = array_msg.header().stamp_sec();
        this->header.stamp.nsec = array_msg.header().stamp_nsec();

        for (auto &data : array_msg.msg_list()) {

            this->msgs_.emplace_back(data);
        }

        return msg_data.length();
    }
}}
