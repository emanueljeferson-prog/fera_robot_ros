source ~/.bashrc
colcon build
source install/setup.bash
ros2 run micro_ros_setup create_agent_ws.sh
ros2 run micro_ros_setup build_agent.sh


sudo rm -rf /home/jefhxmn/Documents/uros_ws/firmware
ros2 run micro_ros_setup create_firmware_ws.sh generate_lib
ros2 run robot_interfaces create_fwws.sh
ros2 run micro_ros_setup build_firmware.sh /home/jefhxmn/Documents/uros_ws/src/pico_toolchain.cmake /home/jefhxmn/Documents/uros_ws/src/pico_colcon.meta

