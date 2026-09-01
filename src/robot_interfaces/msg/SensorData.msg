# Device address 
uint8 address
# Data Timestamp
builtin_interfaces/Time timestamp
# Motor data
int16 motor_speed_left
int16 motor_speed_right
# IMU data
geometry_msgs/Point accel
geometry_msgs/Point gyro
geometry_msgs/Point mag
# Temperature data
int16 temperature 
# Battery data
int16 battery_voltage
int16 battery_percentage
