/**
 * Author: Cyan
 * Date: Wed Dec  8 10:47:46 CST 2021
 */
#pragma once

#include <string>

namespace cmg { namespace std_msgs {

    struct Header {

        struct {

            uint64_t sec{0};

            uint64_t nsec{0};

            auto toSec() const -> double { 

                return this->sec * 1.0 + this->nsec * 1e-9; 
            }

            double operator= (const double t) {

                this->sec = t;
                this->nsec = uint64_t(t * 1e9) % uint64_t(1e9);

                return t;
            }
        } stamp;

        std::string frame_id{""};
    };


    struct Point3 {

        double x, y, z;
    };
}}
