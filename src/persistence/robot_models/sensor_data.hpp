#pragma once 
#include <cstdint>
#include "imu.hpp"
#include "odometry.hpp"
#include "battery.hpp"

namespace robot_models {

struct sensor_data {
    imu imu_data;
    odometry odometry_data;
    battery battery_data;
};

}