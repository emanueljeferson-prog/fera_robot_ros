#pragma once 
#include <cstdint>
#include "timestamp.hpp"

namespace robot_models {

struct odometry {
    timestamp timestamp_value;
    int16_t velocity_left;
    int16_t velocity_right;
};

}