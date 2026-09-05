#include "estimator_node.hpp"

EstimatorNode::EstimatorNode() : Node("estimator_node") {
    subscription_sensor_data_ = this->create_subscription<robot_interfaces::msg::SensorData>(
        "sensor_data", 10,
        std::bind(&EstimatorNode::sensorDataCallback, this, std::placeholders::_1));

    auto configuration_qos = rclcpp::QoS(rclcpp::KeepLast(1));
    configuration_qos.transient_local();
    subscription_configurations_ = this->create_subscription<robot_interfaces::msg::Configurations>(
        "configurations", configuration_qos,
        std::bind(&EstimatorNode::configurationsCallback, this, std::placeholders::_1));
}

EstimatorNode::~EstimatorNode() {}

void EstimatorNode::sensorDataCallback(const robot_interfaces::msg::SensorData::SharedPtr msg) {
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
    processSensorData();
}

void EstimatorNode::configurationsCallback(const robot_interfaces::msg::Configurations::SharedPtr msg) {
    sensorData.configuration_data.accel_offset_x = msg->accel_offset_x;
    sensorData.configuration_data.accel_offset_y = msg->accel_offset_y;
    sensorData.configuration_data.accel_offset_z = msg->accel_offset_z;
    sensorData.configuration_data.gyro_offset_x = msg->gyro_offset_x;
    sensorData.configuration_data.gyro_offset_y = msg->gyro_offset_y;
    sensorData.configuration_data.gyro_offset_z = msg->gyro_offset_z;
    sensorData.configuration_data.mag_offset_x = msg->mag_offset_x;
    sensorData.configuration_data.mag_offset_y = msg->mag_offset_y;
    sensorData.configuration_data.mag_offset_z = msg->mag_offset_z;
    sensorData.configuration_data.accel_scale = msg->accel_scale;
    sensorData.configuration_data.gyro_scale = msg->gyro_scale;
    sensorData.configuration_data.mag_scale = msg->mag_scale;
    sensorData.configuration_data.gnss_reference_lat = msg->gnss_reference_lat;
    sensorData.configuration_data.gnss_reference_lon = msg->gnss_reference_lon;
    sensorData.configuration_data.gnss_reference_alt = msg->gnss_reference_alt;
    sensorData.configuration_data.semi_major_axis = msg->semi_major_axis;
    sensorData.configuration_data.semi_minor_axis = msg->semi_minor_axis;
    sensorData.configuration_data.eccentricity = msg->eccentricity;
    sensorData.configuration_data.flattening = msg->flattening;
    sensorData.configuration_data.gravity = msg->gravity;
    sensorData.configuration_data.magnetic_declination = msg->magnetic_declination;
    sensorData.configuration_data.magnetic_inclination = msg->magnetic_inclination;
    sensorData.configuration_data.magnetic_field_strength = msg->magnetic_field_strength;
    sensorData.configuration_data.scale_factor_encoder = msg->scale_factor_encoder;
    configuration_received_ = true;
    RCLCPP_INFO(this->get_logger(), "Configuracoes recebidas: accel_scale=%.6f, gyro_scale=%.6f",
        msg->accel_scale, msg->gyro_scale);
}

void EstimatorNode::processSensorData() {
    if (!configuration_received_) {
        RCLCPP_WARN_THROTTLE(
            this->get_logger(), *this->get_clock(), 5000,
            "Aguardando a mensagem /configurations antes de processar sensores");
        return;
    }
    float accel_x = (static_cast<float>(sensorData.imu_data.accel_x) - sensorData.configuration_data.accel_offset_x) / sensorData.configuration_data.accel_scale;
    float accel_y = (static_cast<float>(sensorData.imu_data.accel_y) - sensorData.configuration_data.accel_offset_y) / sensorData.configuration_data.accel_scale;
    float accel_z = (static_cast<float>(sensorData.imu_data.accel_z) - sensorData.configuration_data.accel_offset_z) / sensorData.configuration_data.accel_scale;
    float gyro_x = (static_cast<float>(sensorData.imu_data.gyro_x) - sensorData.configuration_data.gyro_offset_x) / sensorData.configuration_data.gyro_scale;
    float gyro_y = (static_cast<float>(sensorData.imu_data.gyro_y) - sensorData.configuration_data.gyro_offset_y) / sensorData.configuration_data.gyro_scale;
    float gyro_z = (static_cast<float>(sensorData.imu_data.gyro_z) - sensorData.configuration_data.gyro_offset_z) / sensorData.configuration_data.gyro_scale;
    float mag_x = (static_cast<float>(sensorData.imu_data.mag_x) - sensorData.configuration_data.mag_offset_x) / sensorData.configuration_data.mag_scale;
    float mag_y = (static_cast<float>(sensorData.imu_data.mag_y) - sensorData.configuration_data.mag_offset_y) / sensorData.configuration_data.mag_scale;
    float mag_z = (static_cast<float>(sensorData.imu_data.mag_z) - sensorData.configuration_data.mag_offset_z) / sensorData.configuration_data.mag_scale;
    RCLCPP_INFO(
        rclcpp::get_logger("EstimatorNode"),
        "Processed Sensor Data: Accel=(%.2f, %.2f, %.2f), Gyro=(%.2f, %.2f, %.2f), Mag=(%.2f, %.2f, %.2f)",
        accel_x, accel_y, accel_z,
        gyro_x, gyro_y, gyro_z,
        mag_x, mag_y, mag_z
    );
}

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<EstimatorNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}