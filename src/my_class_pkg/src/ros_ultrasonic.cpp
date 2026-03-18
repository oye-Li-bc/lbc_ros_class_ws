#include "ros/ros.h"
#include "sensor_msgs/Range.h"
#include "geometry_msgs/Twist.h"
#include "std_msgs/Bool.h"
#include <signal.h>

// 全局退出标记（Ctrl+C秒退）
volatile bool g_quit = false;

// 信号处理函数：Ctrl+C直接退出
void sigint_handler(int sig) {
    ROS_WARN("=== Ctrl+C Pressed! Exit Now ===");
    g_quit = true;
    ros::shutdown();
    exit(0);
}

// 全局变量
ros::Publisher vel_pub;
bool is_avoiding = false;

// 核心配置（按你的要求）
const float FORWARD_SPEED = 0.2;    // 前进速度
const float BACKWARD_SPEED = -0.2;  // 后退速度
const float TURN_SPEED = 0.5;       // 右转速度
const float SAFE_DISTANCE = 0.4;    // 40cm触发阈值
const double BACK_TIME = 1.0;       // 后退1秒（约20cm）
const double TURN_TIME = 0.8;       // 右转0.8秒（约25°）

// 发布速度指令
void publish_vel(float linear_x, float angular_z) {
    if (g_quit) return;
    geometry_msgs::Twist vel_msg;
    vel_msg.linear.x = linear_x;
    vel_msg.angular.z = angular_z;
    vel_pub.publish(vel_msg);
    ROS_INFO("Vel: forward=%.2f m/s, turn=%.2f rad/s", linear_x, angular_z);
}

// 电机使能
void enable_motor() {
    ros::Publisher enable_pub = ros::NodeHandle().advertise<std_msgs::Bool>("/robot/motor/enable", 1);
    std_msgs::Bool enable_msg;
    enable_msg.data = true;
    enable_pub.publish(enable_msg);
    ROS_WARN("=== Motor Enabled ===");
    ros::Duration(0.5).sleep();
}

// 避障动作：后退→右转
void do_avoid() {
    if (is_avoiding || g_quit) return;
    is_avoiding = true;

    // 1. 停止
    publish_vel(0.0, 0.0);
    ros::Duration(0.2).sleep();

    // 2. 后退
    ROS_WARN("=== Front Obstacle (<40cm) → Backward ===");
    publish_vel(BACKWARD_SPEED, 0.0);
    ros::Duration(BACK_TIME).sleep();

    // 3. 右转
    ROS_WARN("=== Turn RIGHT ===");
    publish_vel(0.0, TURN_SPEED);
    ros::Duration(TURN_TIME).sleep();

    // 4. 恢复前进
    publish_vel(FORWARD_SPEED, 0.0);
    is_avoiding = false;
    ROS_WARN("=== Avoidance Done → Forward ===");
}

// 前传感器回调（仅处理前传感器）
void front_sensor_callback(const sensor_msgs::Range::ConstPtr& msg) {
    if (g_quit || is_avoiding) return;

    float front_dist = msg->range;
    ROS_INFO("Front Sensor: %.2f m (Threshold: 0.4m)", front_dist);

    // 仅前传感器＜40cm时触发避障
    if (front_dist > 0.01 && front_dist < SAFE_DISTANCE) {
        do_avoid();
    } else {
        // 其他情况一直前进
        publish_vel(FORWARD_SPEED, 0.0);
    }
}

int main(int argc, char **argv) {
    // 禁用ROS默认信号处理
    ros::init(argc, argv, "w2a_simple_avoidance", ros::init_options::NoSigintHandler);
    ros::NodeHandle nh;

    // 注册Ctrl+C信号处理
    signal(SIGINT, sigint_handler);

    // 订阅前传感器（/ul/sensor2是W2A前超声波）
    ros::Subscriber sub_front = nh.subscribe("/ul/sensor2", 1, front_sensor_callback);

    // 发布速度到正确话题（/cmd_vel）
    vel_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 10);

    // 启动：使能电机+强制前进
    enable_motor();
    publish_vel(FORWARD_SPEED, 0.0);

    ROS_INFO("=== Simple Avoidance Ready ===");
    ROS_INFO("Rule: Front <40cm → Backward + Right Turn");
    ROS_INFO("Press Ctrl+C to exit");

    // 主循环：持续前进（防停住）
    ros::Rate rate(10);
    while (!g_quit && ros::ok()) {
        if (!is_avoiding) {
            publish_vel(FORWARD_SPEED, 0.0);
        }
        ros::spinOnce();
        rate.sleep();
    }

    // 退出前停止小车
    publish_vel(0.0, 0.0);
    ROS_INFO("=== Node Exited Safely ===");
    return 0;
}
