source ~/.bashrc
source install/setup.bash
sudo chmod 666 /dev/ttyACM0
ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyACM0