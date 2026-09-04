#include <queue>
#include "rclcpp/rclcpp.hpp"
#include "robot_interfaces/msg/sensor_data.hpp"
#include "data_base.hpp"
#include "robot_models/sensor_data.hpp"

class PersistenceNode : public rclcpp::Node {
public:
    PersistenceNode();
    ~PersistenceNode();
private:
    void sensorDataCallback(const robot_interfaces::msg::SensorData::SharedPtr msg);
    void databaseWorker();
private:
    std::thread worker_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic<bool> running_;
    std::unique_ptr<Database> db;
    std::queue<robot_models::sensor_data> dataQueue;
    rclcpp::Subscription<robot_interfaces::msg::SensorData>::SharedPtr subscription_;
};