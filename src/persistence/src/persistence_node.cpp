#include "persistence_node.hpp"

PersistenceNode::PersistenceNode() : Node("persistence_node"), running_(true) {
    db = std::make_unique<Database>();
    worker_ = std::thread(&PersistenceNode::databaseWorker, this);
    if (!db->connectDatabase("robot_data.db")) {
        RCLCPP_ERROR(this->get_logger(), "Failed to connect to the database.");
        rclcpp::shutdown();
    }
    subscription_ = 
        this->create_subscription<robot_interfaces
        ::msg
        ::SensorData>("sensor_data", 10, std::bind(&PersistenceNode::sensorDataCallback, this, std::placeholders::_1));
}

PersistenceNode::~PersistenceNode() {
    running_ = false;
    condition_.notify_one();
    if (worker_.joinable()) {
        worker_.join();
    }
}

void PersistenceNode::sensorDataCallback(const robot_interfaces::msg::SensorData::SharedPtr msg) {
    robot_models::sensor_data sensorData;
    sensorData.imu_data.timestamp_value = {static_cast<uint32_t>(msg->timestamp.sec), msg->timestamp.nanosec};
    sensorData.imu_data.accel_x = msg->accel.x;
    sensorData.imu_data.accel_y = msg->accel.y;
    sensorData.imu_data.accel_z = msg->accel.z;
    sensorData.imu_data.gyro_x = msg->gyro.x;
    sensorData.imu_data.gyro_y = msg->gyro.y;
    sensorData.imu_data.gyro_z = msg->gyro.z;
    sensorData.imu_data.mag_x = msg->mag.x;
    sensorData.imu_data.mag_y = msg->mag.y;
    sensorData.imu_data.mag_z = msg->mag.z;
    sensorData.odometry_data.timestamp_value = {static_cast<uint32_t>(msg->timestamp.sec), msg->timestamp.nanosec};
    sensorData.odometry_data.velocity_left = msg->motor_speed_left;
    sensorData.odometry_data.velocity_right = msg->motor_speed_right;
    sensorData.battery_data.timestamp_value = {static_cast<uint32_t>(msg->timestamp.sec), msg->timestamp.nanosec};
    sensorData.battery_data.voltage = msg->battery_voltage;
    sensorData.battery_data.temperature = msg->temperature;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        dataQueue.push(sensorData);
    }
    condition_.notify_one();
}

void PersistenceNode::databaseWorker() {
    while (running_) {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] {
            return !dataQueue.empty() || !running_;
        });
        if (!running_ && dataQueue.empty()) {
            break;
        }
        auto data = dataQueue.front();
        dataQueue.pop();
        lock.unlock();
        db->insertImuData(data.imu_data);
        db->insertOdometryData(data.odometry_data);
        db->insertBatteryData(data.battery_data);
    }
}

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PersistenceNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}