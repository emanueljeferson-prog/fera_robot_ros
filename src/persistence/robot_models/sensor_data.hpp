#pragma once 
#include <cstdint>
#include "imu.hpp"
#include "odometry.hpp"
#include "battery.hpp"
#include "configurations.hpp"

namespace robot_models {

struct sensor_data {
    imu imu_data;
    odometry odometry_data;
    battery battery_data;
    configurations configuration_data;
};

}