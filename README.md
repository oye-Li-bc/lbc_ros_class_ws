# lbc_ros_class_ws

机器人 ROS 课程配套 catkin 工作空间，覆盖**话题/服务/动作、参数、调试可视化、机器人仿真、视觉、激光雷达、SLAM 导航、语音与大模型**等实验。

## 环境

- Ubuntu 20.04 + ROS Noetic
- 课程实物/仿真依赖包：`upros_bringup`、`upros_navigation`、`upros_arm`、`upros_chat`、`upros_move_linear` 等（由课程方提供）
- 系统包：`teleop_twist_keyboard`、`rqt_reconfigure`、`gmapping`、`move_base`

## 仓库结构

```
.
├── src/
│   ├── my_class_pkg/           # 实验主功能包（C++/Python 节点、自定义 msg/srv/action、launch）
│   └── lbc_robot_description/  # 机器人 URDF 与 Gazebo/RViz 启动文件
├── tag_grab.cpp                # 视觉抓取示例代码
├── 2026-03-25-23-02-49.bag     # 录制数据包
└── README.md
```

## 构建

```bash
cd lbc_ros_class_ws
catkin_make
source devel/setup.bash
```

之后每开一个新终端都需要 `source devel/setup.bash`（或写进 `~/.bashrc`）。

---

## 实验索引

> 约定：每条命令 `//` 后是注释或示例输出。多终端实验请按顺序在**新终端**中执行，必要时先 `source devel/setup.bash`。

### 第二周 · 实验一：话题与消息

**5.1.1 C++ 发布者**
```bash
roscore
rosrun my_class_pkg ros_publisher_node
rostopic echo /lbc_topic
```

**5.1.2 C++ 订阅者**
```bash
roscore
rosrun my_class_pkg ros_publisher_node
rosrun my_class_pkg ros_subscriber_node
```

**5.2.1 / 5.2.2 Python 发布者/订阅者**
```bash
roscore
rosrun my_class_pkg ros_publisher_node.py
rostopic echo /my_topic
rosrun my_class_pkg ros_subscriber_node.py
```

**5.3 自定义消息**
```bash
rosmsg show my_class_pkg/MyMessage
# int32 key
# string value
```

**5.5 用 launch 启动节点**
```bash
roslaunch my_class_pkg bringup_topic.launch
```

### 第二周 · 实验一：传感器

```bash
roslaunch upros_bringup bringup_w2a.launch        # 通用启动，下同
rosrun my_class_pkg ros_bump_node                 # 碰撞避障
rosrun my_class_pkg ros_avoid_node                # 超声 TOF 避障
rosrun my_class_pkg ros_imu_spin_node             # IMU 自旋
```

### 第二周 · 实验二：服务

**5.1 自定义服务**
```bash
rossrv show my_class_pkg/MyServiceMsg
# int64 input
# ---
# int64 output
```

**5.2 C++ / Python 服务调用**
```bash
roscore
rosrun my_class_pkg ros_server_node   # 或 ros_server.py
rosrun my_class_pkg ros_client_node   # 或 ros_client.py
```

### 第二周 · 实验二：动作（Action）

```bash
roscore
rosrun my_class_pkg action_server_node   # 或 action_server.py
rosrun my_class_pkg action_client_node   # 或 action_client.py
```

### 第三周 · 实验一：参数与动态参数

**5.1 / 5.2 C++ / Python 读写参数**
```bash
rosrun my_class_pkg ros_param            # C++
rosrun my_class_pkg ros_param.py         # Python
roslaunch my_class_pkg parameter.launch  # 通过 launch 配置参数
```

**5.3 动态参数**
```bash
rosrun my_class_pkg dynamic_reconfigure_node
rosrun rqt_reconfigure rqt_reconfigure
```

**5.5 动态调速**
```bash
roslaunch upros_bringup bringup_w2a.launch
rosrun my_class_pkg ros_dynamic_speed_node
rosrun rqt_reconfigure rqt_reconfigure   # 拖动 double_param 调速
```

### 第三周 · 实验二：调试与可视化

```bash
rosrun my_class_pkg ros_log       # C++ Log 节点
rosrun my_class_pkg ros_log.py    # Python Log 节点
```

### 第四周 · 教具机器人仿真

**URDF + RViz**
```bash
roslaunch lbc_robot_description display.launch
# Global Options → Fixed Frame: base_link
# Add → RobotModel
```

**Gazebo + 键盘遥控**
```bash
roslaunch lbc_robot_description gazebo.launch
rosrun teleop_twist_keyboard teleop_twist_keyboard.py
# U/J/I/K/L 方向，空格急停
```

### 第五周 · 视觉相关实验

```bash
roslaunch upros_bringup bringup_w2a.launch

rosrun my_class_pkg follow_line.py        # 实验二：颜色巡线
rosrun my_class_pkg gesture_movement.py   # 实验三：手势控制（0=闭合，5=张开）
rosrun my_class_pkg apriltag_follow.py    # 实验四：AprilTag 跟踪
```

**实验五：视觉抓取**
```bash
roslaunch upros_bringup bringup_w2a.launch
roslaunch upros_arm recognize_apriltag.launch
rosrun my_class_pkg tag_grab_node
```

### 第六周 · 激光雷达

```bash
roslaunch upros_bringup bringup_w2a.launch
rosrun my_class_pkg ros_scan_node    # 5.5 读取激光数据
rosrun my_class_pkg ros_avoid_node   # 5.7 激光避障
```

### 第六周 · 建图与导航

**建图（Gmapping）**
```bash
roslaunch upros_bringup bringup_w2a.launch
roslaunch upros_navigation gmapping.launch
roslaunch upros_navigation view_nav.launch
rosrun upros_move_linear teleop_twist_keyboard.py
roslaunch upros_navigation save_map.launch
```

**定位 + 导航**
```bash
roslaunch upros_bringup bringup_w2a.launch
roslaunch upros_navigation navigation.launch
roslaunch upros_navigation view_nav.launch
```

**程序化发送导航点**
```bash
rosrun my_class_pkg movebase_client_node   # 自动前往两个目标点
```

### 第七周 · 语音与大模型

```bash
roslaunch upros_chat speech_to_word.launch
rosrun my_class_pkg llm_chat.py
# 运行前请在脚本里把音频输入设备改为麦克风模块
```

---

## 备注

- 仓库根目录的 `2026-03-25-23-02-49.bag` 是课程录制的数据包，可用 `rosbag play` 回放。
- 课程依赖的 `upros_*` 系列功能包未包含在本仓库内，需另行获取并放入 `src/` 后重新 `catkin_make`。
