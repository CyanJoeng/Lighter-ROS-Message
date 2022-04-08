/**
 * Author: Cyan
 * Date: Fri Apr  1 10:29:01 CST 2022
 */
#include <memory>
#include <sstream>

#include "cmg/message/message.hpp"
#include "cmg/utils/log.hpp"
#include "messages/std_msgs/Header.hpp"

namespace cmg { namespace std_msgs {

    class Array : public Message {

        std::vector<std::string> msgs_;

    public:
        Header header;

    public:
        auto addMessage(const Message &message) -> std::size_t;

        auto operator <<(const Message &message) -> Array&;

        virtual auto serialize(std::ostream &out) const -> unsigned long final;

        virtual auto parse(std::istream &in) -> unsigned long final;

        auto size() const -> std::size_t { return this->msgs_.size(); }

        template <typename Msg>
        auto item(int idx) const -> Msg {

            std::stringstream ss;
            ss << this->msgs_.at(idx);

            Msg msg;
            dynamic_cast<Message&>(msg).parse(ss);

            return msg;
        }
    };

    using ArrayPtr = std::shared_ptr<Array>;
    using ArrayConstPtr = std::shared_ptr<const Array>;
}}
