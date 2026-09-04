source /opt/ros/$ROS_DISTRO/setup.bash
colcon build --packages-select robot_interfaces
#colcon build --packages-select persistence --cmake-clean-cache
