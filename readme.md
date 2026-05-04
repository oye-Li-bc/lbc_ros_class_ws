第二周：实验一主题与消息实验

5.1.1 使用C++实现一个发布者发布消息
  roscore
  // 打开新终端
  rosrun my_class_pkg ros_publisher_node
  // 打开新终端
  rostopic echo /lbc_topic
  //data: "LBC ROS\u5B9E\u9A8C - \u53D1\u5E03\u6D88\u606F: 2208"
  
5.1.2 使用C++实现一个订阅者
  roscore
  // 打开新终端
  rosrun my_class_pkg ros_publisher_node
  // 打开新终端
  rosrun my_class_pkg ros_subscriber_node
  //[ INFO] [1777864710.089865465]: ????????????????????????: [LBC ROS?????? - ????????????: 204]
  
5.2.1 使用Python实现一个发布者
  roscore
  // 打开新终端
  rosrun my_class_pkg ros_publisher_node.py
  // 打开新终端
  rostopic echo /my_topic
  //data: "Hello, world!"
  
5.2.2 使用Python实现一个订阅者
  roscore
  // 打开新终端
  rosrun my_class_pkg ros_publisher_node.py
  // 打开新终端
  rosrun my_class_pkg ros_subscriber_node.py
  //[INFO] [1777865909.150505]: /my_subscriber_267606_1777865852177I heard Hello, world!
  
5.3自定义一个ROS消息
  rosmsg show my_class_pkg/MyMessage
  //得到int32 key
  //    string value

5.5实现Launch文件启动节点
  roslaunch my_class_pkg bringup_topic.launch
  //[ INFO] [1777866863.913816878]: LBC ROS?????? - ????????????: 59
  //[ INFO] [1777866863.914770603]: ????????????????????????: [LBC ROS?????? - ????????????: 59]


第二周：实验一传感器实验

5.2编写节点实现基于碰撞传感器的避障
  roslaunch upros_bringup bringup_w2a.launch
  rosrun my_class_pkg ros_bump_node
  
5.2编写节点实现基于超声TOF传感器的避障
  roslaunch upros_bringup bringup_w2a.launch
  rosrun my_class_pkg ros_avoid_node
  
5.2编写节点实现基于IMU的自旋控制
  roslaunch upros_bringup bringup_w2a.launch
  rosrun my_class_pkg ros_imu_spin_node
  
  
第二周：实验二服务实验

5.1自定义一个ROS服务
  rossrv show my_class_pkg/MyServiceMsg
  //int64 input
  //---
  //int64 output

5.2使用C++实现一个ROS服务的传递
  roscore
  //新终端
  rosrun my_class_pkg ros_server_node
  //新终端
  rosrun my_class_pkg ros_client_node
  
5.2使用Python实现一个ROS服务的传递
  roscore
  //新终端
  rosrun my_class_pkg ros_server.py
  //新终端
  rosrun my_class_pkg ros_client.py 
  
 
第二周：实验二动作实验 

5.2使用C++实现一个ROS动作的传递
  roscore
  //新终端
  rosrun my_class_pkg action_server_node
  //新终端
  rosrun my_class_pkg action_client_node
  
5.2使用Python实现一个ROS服务的传递
  roscore
  //新终端
  rosrun my_class_pkg action_server.py
  //新终端
  rosrun my_class_pkg action_client.py
 
 
第三周：实验一参数与动态参数实验

5.1使用C++设置读取参数
  roscore
  //新终端
  rosrun my_class_pkg ros_param
  
5.2使用Python设置读取参数
  roscore
  //新终端
  rosrun my_class_pkg ros_param.py
  //launch文件配置参数
  roslaunch my_class_pkg parameter.launch     
  
5.3配置动态参数并应用
  roscore
  //新终端
  rosrun my_class_pkg dynamic_reconfigure_node
  //新终端
  rosrun rqt_reconfigure rqt_reconfigure
  
5.5动态配置ROS机器人的速度
  roscore
  //新终端  
  roslaunch upros_bringup bringup_w2a.launch
  //新终端
  rosrun my_class_pkg ros_dynamic_speed_node
  //新终端
  rosrun rqt_reconfigure rqt_reconfigure 
  //拖动double_param控制速度
  
  
第三周：实验二ROS调试与可视化工具实验

5.1使用C++实现一个LOG节点
  roscore
  rosrun my_class_pkg ros_log 
  
5.1使用Python实现一个LOG节点
  roscore
  rosrun my_class_pkg ros_log.py
  
第四周：教具机器人仿真功能使用

1.用urdf编写一个机器人形态用rviz查看
  roslaunch lbc_robot_description display.launch
  //Global Options → Fixed Frame → 改为 base_link
  //左下方 Add → RobotModel → OK
  //完成后你会看到：蓝色底盘 + 两个黑轮子 + 灰色万向轮
  
2.启动gazebo，键盘控制
  roslaunch lbc_robot_description gazebo.launch
  //新终端
  rosrun teleop_twist_keyboard teleop_twist_keyboard.py
  //U/J/I/K/L 控制方向
  //空格急停


第五周：实验二基于颜色识别的自主巡线实验

  roslaunch upros_bringup bringup_w2a.launch
  rosrun my_class_pkg follow_line.py
  
  
第五周：实验三基于手势识别的机器人控制实验

  roslaunch upros_bringup bringup_w2a.launch
  rosrun my_class_pkg gesture_movement.py
  //“0”闭合，“5”张开
  
  
第五周：实验四视觉跟踪实验

  roslaunch upros_bringup bringup_w2a.launch
  rosrun my_class_pkg apriltag_follow.py
  //机器人可以跟着块转向和移动  
  
  
第五周：实验五视觉抓取实验

  roslaunch upros_bringup bringup_w2a.launch
  roslaunch upros_arm recognize_apriltag.launch
  rosrun my_class_pkg tag_grab_node
    
    
第六周：激光雷达驱动实验

5.5激光雷达信息ROS获取
  roslaunch upros_bringup bringup_w2a.launch
  rosrun my_class_pkg ros_scan_node
  
5.7激光雷达避障
  roslaunch upros_bringup bringup_w2a.launch
  rosrun my_class_pkg ros_avoid_node
  
第六周：建图导航实验

5.1机器人建图实验
  roslaunch upros_bringup bringup_w2a.launch
  roslaunch upros_navigation gmapping.launch
  //启动Gmapping
  roslaunch upros_navigation view_nav.launch
  //rviz中观察地图
  rosrun upros_move_linear teleop_twist_keyboard.py
  //键盘遥控
  roslaunch upros_navigation save_map.launch
  //保存地图
  
5.2机器人定位导航实验
  roslaunch upros_bringup bringup_w2a.launch
  roslaunch upros_navigation navigation.launch
  //启动导航节点
  roslaunch upros_navigation view_nav.launch
  //观察导航效果
  
5.2机器人发送导航点
  roslaunch upros_bringup bringup_w2a.launch
  roslaunch upros_navigation navigation.launch
  roslaunch upros_navigation view_nav.launch
  rosrun my_class_pkg movebase_client_node
  //给定两个点让机器人自动前往
  
  
第七周：语音交互与大模型实验
  
5.3大模型调用
  roslaunch upros_chat speech_to_word.launch
  rosrun my_class_pkg llm_chat.py
  //使用前记得设置里面把麦克风改成麦克风模块
