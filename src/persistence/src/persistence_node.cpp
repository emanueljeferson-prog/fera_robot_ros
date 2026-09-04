#include "rclcpp/rclcpp.hpp"
#include "robot_interfaces/msg/sensor_data.hpp"
#include "data_base.hpp"



class PersistenceNode : public rclcpp::Node {
public:
    PersistenceNode() : Node("persistence_node") {
        db = std::make_unique<Database>();
        if (!db->connectDatabase("robot_data.db")) {
            RCLCPP_ERROR(this->get_logger(), "Failed to connect to the database.");
            rclcpp::shutdown();
        }

        subscription_ = this->create_subscription<robot_interfaces::msg::SensorData>(
            "sensor_data", 10,
            std::bind(&PersistenceNode::sensorDataCallback, this, std::placeholders::_1));
    }

private:
    std::unique_ptr<Database> db;
    rclcpp::Subscription<robot_interfaces::msg::SensorData>::SharedPtr subscription_;
    void sensorDataCallback(const robot_interfaces::msg::SensorData::SharedPtr msg) {
        robot_models::imu imuData;
        imuData.timestamp_value = {static_cast<uint32_t>(msg->timestamp.sec),
                       msg->timestamp.nanosec};
        imuData.accel_x = msg->accel.x;
        imuData.accel_y = msg->accel.y;
        imuData.accel_z = msg->accel.z;
        imuData.gyro_x = msg->gyro.x;
        imuData.gyro_y = msg->gyro.y;
        imuData.gyro_z = msg->gyro.z;
        imuData.mag_x = msg->mag.x;
        imuData.mag_y = msg->mag.y;
        imuData.mag_z = msg->mag.z;
        db->insertImuData(imuData);

        robot_models::odometry odometryData;
        odometryData.timestamp_value = {static_cast<uint32_t>(msg->timestamp.sec),
                        msg->timestamp.nanosec};
        odometryData.velocity_left = msg->motor_speed_left;
        odometryData.velocity_right = msg->motor_speed_right;
        db->insertOdometryData(odometryData);

        robot_models::battery batteryData;
        batteryData.timestamp_value = {static_cast<uint32_t>(msg->timestamp.sec),
                           msg->timestamp.nanosec};
        batteryData.voltage = msg->battery_voltage;
        batteryData.temperature = msg->temperature;
        db->insertBatteryData(batteryData);
    }
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PersistenceNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}