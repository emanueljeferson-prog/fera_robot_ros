#include <string>
#include "sqlite3.h"
#include "robot_models/imu.hpp"
#include "robot_models/odometry.hpp"
#include "robot_models/battery.hpp"

class Database {
public:
    Database();
    ~Database();
    bool connectDatabase(const std::string& dbPath);
    void insertImuData(const robot_models::imu& imuData);
    void insertOdometryData(const robot_models::odometry& odometryData);
    void insertBatteryData(const robot_models::battery& batteryData);
private:
    sqlite3* db = nullptr;
};