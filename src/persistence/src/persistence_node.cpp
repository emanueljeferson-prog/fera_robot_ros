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
    rclcpp::QoS qos(rclcpp::KeepLast(1));
    qos.transient_local();
    publisher_ = this->create_publisher<robot_interfaces::msg::Configurations>("configurations", qos);
    publishConfigurations();   
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

void PersistenceNode::publishConfigurations() {
    robot_models::configurations configData;
    db->getConfigurations(configData);
    auto msg = robot_interfaces::msg::Configurations();
    msg.accel_offset_x = configData.accel_offset_x;
    msg.accel_offset_y = configData.accel_offset_y;
    msg.accel_offset_z = configData.accel_offset_z;
    msg.gyro_offset_x = configData.gyro_offset_x;
    msg.gyro_offset_y = configData.gyro_offset_y;
    msg.gyro_offset_z = configData.gyro_offset_z;
    msg.mag_offset_x = configData.mag_offset_x;
    msg.mag_offset_y = configData.mag_offset_y;
    msg.mag_offset_z = configData.mag_offset_z;
    msg.accel_scale = configData.accel_scale;
    msg.gyro_scale = configData.gyro_scale;
    msg.mag_scale = configData.mag_scale;
    msg.gnss_reference_lat = configData.gnss_reference_lat;
    msg.gnss_reference_lon = configData.gnss_reference_lon;
    msg.gnss_reference_alt = configData.gnss_reference_alt;
    msg.semi_major_axis = configData.semi_major_axis;
    msg.semi_minor_axis = configData.semi_minor_axis;
    msg.eccentricity = configData.eccentricity;
    msg.flattening = configData.flattening;
    msg.gravity = configData.gravity;
    msg.magnetic_declination = configData.magnetic_declination;
    msg.magnetic_inclination = configData.magnetic_inclination;
    msg.magnetic_field_strength = configData.magnetic_field_strength;
    msg.scale_factor_encoder = configData.scale_factor_encoder;
    publisher_->publish(msg);
}

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PersistenceNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}