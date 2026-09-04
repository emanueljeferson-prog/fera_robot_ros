#pragma once 
#include <cstdint>
#include "timestamp.hpp"

namespace robot_models {

struct imu {
    timestamp timestamp_value;
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    int16_t mag_x;
    int16_t mag_y;
    int16_t mag_z;
};

}


