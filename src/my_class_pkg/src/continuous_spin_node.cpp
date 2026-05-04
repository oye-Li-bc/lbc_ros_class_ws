#include "ros/ros.h"
#include "sensor_msgs/Imu.h"
#include "geometry_msgs/Twist.h"
#include "std_msgs/Bool.h"
#include <signal.h>
#include <cmath>
#include <tf/transform_datatypes.h>

// 全局退出标记
volatile bool g_quit = false;

// ROS发布器
ros::Publisher vel_pub;

// 核心配置
const float TURN_SPEED = 0.5;
const std::string IMU_TOPIC = "/imu/data";
const std::string CMD_VEL_TOPIC = "/cmd_vel";

// Ctrl+C 安全退出
void sigint_handler(int sig) {
    ROS_WARN("=== [Continuous Spin] Ctrl+C Pressed! Stop & Exit ===");
    g_quit = true;

    geometry_msgs::Twist stop_msg;
    stop_msg.linear.x = 0.0;
    stop_msg.angular.z = 0.0;
    vel_pub.publish(stop_msg);

    ros::shutdown();
    exit(0);
}

// IMU 回调（打印角度）
void imu_data_callback(const sensor_msgs::Imu::ConstPtr& imu_msg) {
    if (g_quit) return;
    static float accumulated_angle = 0.0;
    static ros::Time last_time;

    if (last_time.isValid()) {
        double dt = (ros::Time::now() - last_time).toSec();
        accumulated_angle += imu_msg->angular_velocity.z * dt;
        ROS_INFO_THROTTLE(1, "Spinning... Total Angle: %.1f deg", accumulated_angle * 180 / M_PI);
    }
    last_time = ros::Time::now();
}

// 连续无限自旋
void continuous_spin() {
    ROS_WARN("=== [Continuous Spin] Start INFINITE SPIN ===");
    ros::Rate rate(50);

    while (!g_quit && ros::ok()) {
        geometry_msgs::Twist vel_msg;
        vel_msg.linear.x = 0.0;
        vel_msg.angular.z = TURN_SPEED;
        vel_pub.publish(vel_msg);

        ros::spinOnce();
        rate.sleep();
    }

    geometry_msgs::Twist stop_msg;
    stop_msg.linear.x = 0.0;
    stop_msg.angular.z = 0.0;
    vel_pub.publish(stop_msg);
    ROS_WARN("=== [Continuous Spin] Spin Stopped ===");
}

// 电机使能
void enable_motor() {
    ros::Publisher enable_pub = ros::NodeHandle().advertise<std_msgs::Bool>("/robot/motor/enable", 1);
    std_msgs::Bool enable_msg;
    enable_msg.data = true;

    for (int i = 0; i < 3; i++) {
        enable_pub.publish(enable_msg);
        ros::Duration(0.1).sleep();
    }
    ROS_WARN("=== [Continuous Spin] Motor Enabled ===");
    ros::Duration(0.5).sleep();
}

// 主函数
int main(int argc, char **argv) {
    // 节点名：continuous_spin_node（以node结尾）
    ros::init(argc, argv, "continuous_spin_node", ros::init_options::NoSigintHandler);
    ros::NodeHandle nh;

    signal(SIGINT, sigint_handler);

    ros::Subscriber imu_sub = nh.subscribe(IMU_TOPIC, 10, imu_data_callback);
    vel_pub = nh.advertise<geometry_msgs::Twist>(CMD_VEL_TOPIC, 10);

    enable_motor();

    ROS_INFO("=== [Continuous Spin Node] Started ===");
    continuous_spin();

    ROS_INFO("=== [Continuous Spin Node] Exited ===");
    return 0;
}

