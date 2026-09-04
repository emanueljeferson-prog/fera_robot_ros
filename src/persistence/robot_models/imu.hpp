#pragma once 
#include <cstdint>
#include "timestamp.hpp"

namespace robot_models {

struct imu {
    timestamp timestamp_value;
    uint16_t accel_x;
    uint16_t accel_y;
    uint16_t accel_z;
    uint16_t gyro_x;
    uint16_t gyro_y;
    uint16_t gyro_z;
    uint16_t mag_x;
    uint16_t mag_y;
    uint16_t mag_z;
};

}


