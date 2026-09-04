#pragma once 
#include <cstdint>
#include "timestamp.hpp"

namespace robot_models {

struct battery {
    timestamp timestamp_value;
    uint16_t voltage;
    uint16_t temperature;
};

}