#pragma once 
#include <cstdint>
#include "timestamp.hpp"

namespace robot_models {

struct gnss {
    timestamp timestamp_value;
    uint16_t latitude;
    uint16_t longitude;
    uint16_t elevation;
};

}