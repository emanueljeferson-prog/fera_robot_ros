#include "data_base.hpp"
#include "rclcpp/rclcpp.hpp"

Database::Database() {}

Database::~Database() {
    /*if (db) {
        sqlite3_close(db);
    }*/
}

bool Database::connectDatabase(const std::string& dbPath) {
    int rc = sqlite3_open(dbPath.c_str(), &this->db);
    if (rc) {
        RCLCPP_ERROR(rclcpp::get_logger("Database"), "Can't open database: %s", sqlite3_errmsg(this->db));
        sqlite3_close(this->db);
        this->db = nullptr;
        return false;
    }
    return true;
}

void Database::insertImuData(const robot_models::imu& imuData) {
    const char* sql = "INSERT INTO imu "
        "(timestamp_sec, timestamp_nanosec, accel_x, accel_y, accel_z, "
        "gyro_x, gyro_y, gyro_z, mag_x, mag_y, mag_z) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK) {
        return;
    }
    sqlite3_bind_int64(statement, 1, imuData.timestamp_value.sec);
    sqlite3_bind_int64(statement, 2, imuData.timestamp_value.nanosec);
    sqlite3_bind_int(statement, 3, imuData.accel_x);
    sqlite3_bind_int(statement, 4, imuData.accel_y);
    sqlite3_bind_int(statement, 5, imuData.accel_z);
    sqlite3_bind_int(statement, 6, imuData.gyro_x);
    sqlite3_bind_int(statement, 7, imuData.gyro_y);
    sqlite3_bind_int(statement, 8, imuData.gyro_z);
    sqlite3_bind_int(statement, 9, imuData.mag_x);
    sqlite3_bind_int(statement, 10, imuData.mag_y);
    sqlite3_bind_int(statement, 11, imuData.mag_z);
    RCLCPP_INFO(
        rclcpp::get_logger("Database"),
        "Saving IMU: timestamp=%u.%09u accel=(%u, %u, %u) gyro=(%u, %u, %u) mag=(%u, %u, %u)",
        imuData.timestamp_value.sec,
        imuData.timestamp_value.nanosec,
        imuData.accel_x,
        imuData.accel_y,
        imuData.accel_z,
        imuData.gyro_x,
        imuData.gyro_y,
        imuData.gyro_z,
        imuData.mag_x,
        imuData.mag_y,
        imuData.mag_z);
    sqlite3_step(statement);
    sqlite3_finalize(statement);
}

void Database::insertOdometryData(const robot_models::odometry& odometryData) {
    const char* sql = "INSERT INTO odometry "
        "(timestamp_sec, timestamp_nanosec, velocity_left, velocity_right) "
        "VALUES (?, ?, ?, ?);";
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK) {
        return;
    }
    sqlite3_bind_int64(statement, 1, odometryData.timestamp_value.sec);
    sqlite3_bind_int64(statement, 2, odometryData.timestamp_value.nanosec);
    sqlite3_bind_int(statement, 3, odometryData.velocity_left);
    sqlite3_bind_int(statement, 4, odometryData.velocity_right);
    RCLCPP_INFO(
        rclcpp::get_logger("Database"),
        "Saving odometry: timestamp=%u.%09u velocity_left=%d velocity_right=%d",
        odometryData.timestamp_value.sec,
        odometryData.timestamp_value.nanosec,
        odometryData.velocity_left,
        odometryData.velocity_right);
    sqlite3_step(statement);
    sqlite3_finalize(statement);
}

void Database::insertBatteryData(const robot_models::battery& batteryData) {
    const char* sql = "INSERT INTO system "
        "(timestamp_sec, timestamp_nanosec, temperature, battery_voltage) "
        "VALUES (?, ?, ?, ?);";
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK) {
        return;
    }
    sqlite3_bind_int64(statement, 1, batteryData.timestamp_value.sec);
    sqlite3_bind_int64(statement, 2, batteryData.timestamp_value.nanosec);
    sqlite3_bind_int(statement, 3, batteryData.temperature);
    sqlite3_bind_int(statement, 4, batteryData.voltage);
    RCLCPP_INFO(
        rclcpp::get_logger("Database"),
        "Saving battery: timestamp=%u.%09u temperature=%u voltage=%u",
        batteryData.timestamp_value.sec,
        batteryData.timestamp_value.nanosec,
        batteryData.temperature,
        batteryData.voltage);
    sqlite3_step(statement);
    sqlite3_finalize(statement);
}

void Database::getConfigurations(robot_models::configurations& configData) {
    const char* sql = "SELECT accel_offset_x, accel_offset_y, accel_offset_z, gyro_offset_x, gyro_offset_y, gyro_offset_z, mag_offset_x, mag_offset_y, mag_offset_z, accel_scale, gyro_scale, mag_scale, gnss_reference_lat, gnss_reference_lon, gnss_reference_alt, semi_major_axis, semi_minor_axis, eccentricity, flattening, gravity, magnetic_declination, magnetic_inclination, magnetic_field_strength, scale_factor_encoder FROM configurations LIMIT 1;";
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK) {
        return;
    }
    if (sqlite3_step(statement) == SQLITE_ROW) {
        configData.accel_offset_x = sqlite3_column_double(statement,0);
        configData.accel_offset_y = sqlite3_column_double(statement,1);
        configData.accel_offset_z = sqlite3_column_double(statement,2);
        configData.gyro_offset_x = sqlite3_column_double(statement,3);
        configData.gyro_offset_y = sqlite3_column_double(statement,4);
        configData.gyro_offset_z = sqlite3_column_double(statement,5);
        configData.mag_offset_x = sqlite3_column_double(statement,6);
        configData.mag_offset_y = sqlite3_column_double(statement,7);
        configData.mag_offset_z = sqlite3_column_double(statement,8);
        configData.accel_scale = sqlite3_column_double(statement,9);
        configData.gyro_scale = sqlite3_column_double(statement,10);
        configData.mag_scale = sqlite3_column_double(statement,11);
        configData.gnss_reference_lat = sqlite3_column_double(statement,12);
        configData.gnss_reference_lon = sqlite3_column_double(statement,13);
        configData.gnss_reference_alt = sqlite3_column_double(statement,14);
        configData.semi_major_axis = sqlite3_column_double(statement,15);
        configData.semi_minor_axis = sqlite3_column_double(statement,16);
        configData.eccentricity = sqlite3_column_double(statement,17);
        configData.flattening = sqlite3_column_double(statement,18);
        configData.gravity = sqlite3_column_double(statement,19);
        configData.magnetic_declination = sqlite3_column_double(statement,20);
        configData.magnetic_inclination = sqlite3_column_double(statement,21);
        configData.magnetic_field_strength = sqlite3_column_double(statement,22);
        configData.scale_factor_encoder = sqlite3_column_double(statement,23);
    }
    sqlite3_finalize(statement);
}
