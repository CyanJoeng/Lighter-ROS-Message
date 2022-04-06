/**
 * Author: Cyan
 * Date: Fri Apr  1 10:29:01 CST 2022
 */
#include <sstream>

#include "cmg/message/message.hpp"
#include "cmg/utils/log.hpp"
#include "messages/std_msgs/Header.hpp"

namespace cmg { namespace std_msgs {

    class Array : public Message {

            std::list<std::string> msg_list;
        
        public:
            struct Iter : public std::list<std::string>::iterator {
                
                Iter(const std::list<std::string>::iterator &it) : std::list<std::string>::iterator(it) {}

                template <typename Msg>
                auto value() -> Msg {

                    auto data = std::list<std::string>::iterator::operator *();

                    std::stringstream ss;
                    ss << data;

                    Msg msg;
                    dynamic_cast<Message&>(msg).parse(ss);

                    return msg;
                }
            };

        public:
            Header header;

        public:
            auto addMessage(const Message &message) -> std::size_t;

            auto operator <<(const Message &message) -> Array&;

            virtual auto serialize(std::ostream &out) const -> unsigned long final;

            virtual auto parse(std::istream &in) -> unsigned long final;

            auto begin() -> Iter {
                return this->msg_list.begin();
            }

            auto end() -> Iter {
                return this->msg_list.end();
            }
     };
}}
