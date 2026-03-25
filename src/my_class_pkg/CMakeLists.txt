cmake_minimum_required(VERSION 3.5)
project(my_class_pkg)

# 1. 查找依赖（添加actionlib/actionlib_msgs）
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  message_generation
  actionlib
  actionlib_msgs
  dynamic_reconfigure
)

# 2. 添加消息/服务/动作文件
add_message_files(DIRECTORY msg FILES MyMessage.msg)
add_service_files(DIRECTORY srv FILES MyServiceMsg.srv)
add_action_files(DIRECTORY action FILES MyActionMsg.action)

# 3. 生成消息/服务/动作（依赖std_msgs+actionlib_msgs）
generate_messages(
  DEPENDENCIES
  std_msgs
  actionlib_msgs
)

# 2. 添加generate_dynamic_reconfigure_options规则
generate_dynamic_reconfigure_options(
  cfg/Tutorials.cfg
)
# 4. catkin包配置
catkin_package(
  CATKIN_DEPENDS roscpp rospy std_msgs message_runtime actionlib actionlib_msgs
)

# 5. 包含目录
include_directories(${catkin_INCLUDE_DIRS})

# 6. 编译之前的节点（消息/服务）
add_executable(ros_server_node src/ros_server.cpp)
add_dependencies(ros_server_node ${${PROJECT_NAME}_EXPORTED_TARGETS})
target_link_libraries(ros_server_node ${catkin_LIBRARIES})

add_executable(ros_client_node src/ros_client.cpp)
add_dependencies(ros_client_node ${${PROJECT_NAME}_EXPORTED_TARGETS})
target_link_libraries(ros_client_node ${catkin_LIBRARIES})

# 7. 编译C++动作服务器/客户端
add_executable(action_server_node src/action_server.cpp)
add_dependencies(action_server_node ${${PROJECT_NAME}_EXPORTED_TARGETS})
target_link_libraries(action_server_node ${catkin_LIBRARIES})

add_executable(action_client_node src/action_client.cpp)
add_dependencies(action_client_node ${${PROJECT_NAME}_EXPORTED_TARGETS})
target_link_libraries(action_client_node ${catkin_LIBRARIES})

# 编译碰撞传感器节点
add_executable(ros_bump_node src/ros_bump.cpp)
add_dependencies(ros_bump_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(ros_bump_node ${catkin_LIBRARIES})

# W2A机器人超声波避障节点
add_executable(ros_ultrasonic_node src/ros_ultrasonic.cpp)
 
# 添加IMU节点编译
add_executable(ros_imu_node src/ros_imu_node.cpp)
target_link_libraries(ros_imu_node ${catkin_LIBRARIES})
add_dependencies(ros_ultrasonic_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(ros_ultrasonic_node ${catkin_LIBRARIES})

# IMU 180°自旋节点编译规则
add_executable(ros_imu_spin_node src/ros_imu_spin_node.cpp)
target_link_libraries(ros_imu_spin_node ${catkin_LIBRARIES})

# cpp读取参数节点编译
add_executable(ros_param src/ros_param.cpp)
target_link_libraries(ros_param ${catkin_LIBRARIES})

#添加动态参数编译规则
add_executable(dynamic_reconfigure_node src/dynamic_reconfigure.cpp)
add_dependencies(dynamic_reconfigure_node ${PROJECT_NAME}_gencfg)
target_link_libraries(dynamic_reconfigure_node ${catkin_LIBRARIES})

#动态配置小乌龟速度
add_executable(ros_dynamic_speed_node src/ros_dynamic_speed.cpp)
add_dependencies(ros_dynamic_speed_node ${PROJECT_NAME}_gencfg)
target_link_libraries(ros_dynamic_speed_node ${catkin_LIBRARIES})



