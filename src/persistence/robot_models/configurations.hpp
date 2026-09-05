#pragma once 
#include <cstdint>

namespace robot_models {

struct configurations {
    double accel_offset_x;
    double accel_offset_y;
    double accel_offset_z;
    double gyro_offset_x;
    double gyro_offset_y;
    double gyro_offset_z;
    double mag_offset_x;
    double mag_offset_y;
    double mag_offset_z;
    double accel_scale;
    double gyro_scale;
    double mag_scale;
    double gnss_reference_lat;
    double gnss_reference_lon;
    double gnss_reference_alt;
    double semi_major_axis;
    double semi_minor_axis;
    double eccentricity;
    double flattening;
    double gravity;
    double magnetic_declination;
    double magnetic_inclination;
    double magnetic_field_strength;
    double scale_factor_encoder;
};

}