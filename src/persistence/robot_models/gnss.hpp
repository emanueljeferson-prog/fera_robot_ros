#pragma once 
#include <cstdint>
#include "timestamp.hpp"

namespace robot_models {

struct gnss {
    timestamp timestamp_value;
    int16_t latitude;
    int16_t longitude;
    int16_t elevation;
};

}