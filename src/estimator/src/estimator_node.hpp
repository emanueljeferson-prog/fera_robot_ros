#include <queue>
#include "rclcpp/rclcpp.hpp"
#include "robot_interfaces/msg/configurations.hpp"
#include "robot_interfaces/msg/sensor_data.hpp"
#include "robot_models/sensor_data.hpp"

class EstimatorNode : public rclcpp::Node {
public:
    EstimatorNode();
    ~EstimatorNode();
    void processSensorData();
private:
    void sensorDataCallback(const robot_interfaces::msg::SensorData::SharedPtr msg);
    void configurationsCallback(const robot_interfaces::msg::Configurations::SharedPtr msg);
private:
    robot_models::sensor_data sensorData;
    bool configuration_received_{false};
    rclcpp::Subscription<robot_interfaces::msg::SensorData>::SharedPtr subscription_sensor_data_;
    rclcpp::Subscription<robot_interfaces::msg::Configurations>::SharedPtr subscription_configurations_;
};