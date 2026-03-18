#include "ros/ros.h"
#include "sensor_msgs/Imu.h"
#include "geometry_msgs/Twist.h"
#include "std_msgs/Bool.h"
#include <signal.h>
#include <cmath>
#include <tf/transform_datatypes.h>

// 全局退出标记（Ctrl+C秒退）
volatile bool g_quit = false;

// IMU核心数据
float imu_angular_z = 0.0;    // 绕z轴角速度 (rad/s)
float accumulated_angle = 0.0;// 累计转向角度 (rad)
ros::Time last_imu_time;      // 上一次IMU数据时间戳

// ROS发布器
ros::Publisher vel_pub;       // 速度指令发布器

// 核心配置（严格按流程图要求）
const float TURN_SPEED = 0.5;       // 转向角速度 (rad/s)
const float TARGET_ANGLE = M_PI;    // 目标转向角度：180°（弧度）
const std::string IMU_TOPIC = "/imu/data";    // IMU数据话题
const std::string CMD_VEL_TOPIC = "/cmd_vel"; // 速度控制话题

// ===================== 信号处理函数（Ctrl+C安全退出） =====================
void sigint_handler(int sig) {
    ROS_WARN("=== [IMU Spin Node] Ctrl+C Pressed! Stop & Exit ===");
    g_quit = true;
    
    // 退出前强制停止小车
    geometry_msgs::Twist stop_msg;
    stop_msg.linear.x = 0.0;
    stop_msg.angular.z = 0.0;
    vel_pub.publish(stop_msg);
    
    ros::shutdown();
    exit(0);
}

// ===================== IMU数据回调（读取IMU数据） =====================
void imu_data_callback(const sensor_msgs::Imu::ConstPtr& imu_msg) {
    if (g_quit) return;

    // 1. 解析IMU绕z轴角速度（流程图：读取IMU数据）
    imu_angular_z = imu_msg->angular_velocity.z;

    // 2. 计算累计转向角度（角速度积分）
    if (last_imu_time.isValid()) {
        double time_diff = (ros::Time::now() - last_imu_time).toSec();
        accumulated_angle += imu_angular_z * time_diff;
    }
    last_imu_time = ros::Time::now();

    // 实时打印角度（调试用）
    ROS_INFO_THROTTLE(0.5, "Current Angle: %.1f deg (Target: 180 deg)", 
                      fabs(accumulated_angle) * 180 / M_PI);
}

// ===================== 核心功能：180°自旋（严格按流程图） =====================
void spin_180_deg() {
    ROS_WARN("=== [IMU Spin Node] Start 180° Spin ===");
    accumulated_angle = 0.0; // 重置累计角度

    ros::Rate rate(100); // 100Hz高频检查角度

    // 流程图：自旋 → 判断是否到180° → 否则继续
    while (!g_quit && fabs(accumulated_angle) < TARGET_ANGLE) {
        // 发布转向指令
        geometry_msgs::Twist vel_msg;
        vel_msg.linear.x = 0.0;                    // 自旋时不前进
        vel_msg.angular.z = TURN_SPEED;            // 右转（如需左转改-TURN_SPEED）
        vel_pub.publish(vel_msg);

        ros::spinOnce(); // 处理IMU回调，更新角度
        rate.sleep();
    }

    // 流程图：达到180° → 停止
    geometry_msgs::Twist stop_msg;
    stop_msg.linear.x = 0.0;
    stop_msg.angular.z = 0.0;
    vel_pub.publish(stop_msg);

    ROS_WARN("=== [IMU Spin Node] 180° Spin Done! Actual Angle: %.1f deg ===",
             fabs(accumulated_angle) * 180 / M_PI);
}

// ===================== 电机使能函数 =====================
void enable_motor() {
    ros::Publisher enable_pub = ros::NodeHandle().advertise<std_msgs::Bool>("/robot/motor/enable", 1);
    std_msgs::Bool enable_msg;
    enable_msg.data = true;
    
    // 多次发布确保使能生效
    for (int i = 0; i < 3; i++) {
        enable_pub.publish(enable_msg);
        ros::Duration(0.1).sleep();
    }
    ROS_WARN("=== [IMU Spin Node] Motor Enabled ===");
    ros::Duration(0.5).sleep();
}

// ===================== 主函数（节点入口） =====================
int main(int argc, char **argv) {
    // 初始化ROS节点（禁用默认信号处理）
    ros::init(argc, argv, "w2a_imu_spin_node", ros::init_options::NoSigintHandler);
    ros::NodeHandle nh;

    // 注册Ctrl+C信号处理
    signal(SIGINT, sigint_handler);

    // 初始化订阅器/发布器
    ros::Subscriber imu_sub = nh.subscribe(IMU_TOPIC, 10, imu_data_callback);
    vel_pub = nh.advertise<geometry_msgs::Twist>(CMD_VEL_TOPIC, 10);

    // 电机使能
    enable_motor();

    // 打印启动信息
    ROS_INFO("=== [IMU Spin Node] Started (Follow Flow Chart) ===");
    ROS_INFO("1. Read IMU Data → 2. Spin → 3. Stop at 180°");
    ROS_INFO("Press Ctrl+C to exit");

    // 自动执行180°自旋（严格按流程图：启动即执行）
    spin_180_deg();

    // 自旋完成后，保持节点存活（可按Ctrl+C退出）
    ros::Rate rate(10);
    while (!g_quit && ros::ok()) {
        ros::spinOnce();
        rate.sleep();
    }

    ROS_INFO("=== [IMU Spin Node] Exited Safely ===");
    return 0;
}
