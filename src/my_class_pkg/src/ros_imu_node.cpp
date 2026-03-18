#include "ros/ros.h"
#include "sensor_msgs/Imu.h"
#include "geometry_msgs/Twist.h"
#include "std_msgs/Bool.h"
#include <signal.h>
#include <cmath>

// 全局退出标记（Ctrl+C秒退）
volatile bool g_quit = false;

// IMU核心数据存储
float imu_angular_z = 0.0;    // 绕z轴角速度 (rad/s)，右转为正
float imu_linear_x = 0.0;     // x轴线加速度 (m/s²)
float accumulated_angle = 0.0;// 累计转向角度 (rad)
ros::Time last_imu_timestamp; // 上一次IMU数据时间戳

// ROS发布器/订阅器
ros::Publisher vel_pub;       // 速度发布器（可选：用于IMU辅助控制）
ros::Subscriber imu_sub;      // IMU订阅器

// 配置参数
const float TARGET_TURN_ANGLE = 25 * M_PI / 180; // 目标转向角度（25°）
const float TURN_SPEED = 0.5;                    // 转向角速度 (rad/s)
const std::string IMU_TOPIC = "/imu/data";       // IMU话题（根据实际修改）
const std::string CMD_VEL_TOPIC = "/cmd_vel";    // 速度话题

// ===================== 信号处理函数 =====================
void sigint_handler(int sig) {
    ROS_WARN("=== [IMU Node] Ctrl+C Pressed! Exit Now ===");
    g_quit = true;
    ros::shutdown();
    exit(0);
}

// ===================== IMU数据解析回调 =====================
void imu_data_callback(const sensor_msgs::Imu::ConstPtr& imu_msg) {
    if (g_quit) return;

    // 1. 解析IMU核心数据
    imu_angular_z = imu_msg->angular_velocity.z;  // 绕z轴角速度（转向核心）
    imu_linear_x = imu_msg->linear_acceleration.x;// x轴线加速度（前进/后退）

    // 2. 计算累计转向角度（仅转向时有效）
    if (last_imu_timestamp.isValid()) {
        double time_diff = (ros::Time::now() - last_imu_timestamp).toSec();
        accumulated_angle += imu_angular_z * time_diff; // 角度=角速度×时间
    }
    last_imu_timestamp = ros::Time::now();

    // 3. 打印IMU数据（调试用）
    ROS_INFO_THROTTLE(1.0,  // 每秒打印1次，避免刷屏
        "=== IMU Data ===\n"
        "Angular Z (Turn Rate): %.2f rad/s (%.1f °/s)\n"
        "Linear X (Acc): %.2f m/s²\n"
        "Accumulated Angle: %.1f °",
        imu_angular_z, imu_angular_z * 180 / M_PI,
        imu_linear_x,
        accumulated_angle * 180 / M_PI
    );
}

// ===================== 基于IMU的精准转向函数 =====================
void precise_turn(int turn_direction) {
    /*
    turn_direction: 1=右转, -1=左转
    功能：基于IMU累计角度，精准转向指定角度（25°）
    */
    if (g_quit) return;
    ROS_WARN("=== Start %s Turn (Target: 25°) ===", turn_direction==1?"RIGHT":"LEFT");

    // 重置累计角度
    accumulated_angle = 0.0;
    ros::Rate rate(100); // 100Hz高频检查角度

    // 转向循环：直到达到目标角度
    while (!g_quit && fabs(accumulated_angle) < TARGET_TURN_ANGLE) {
        geometry_msgs::Twist vel_msg;
        vel_msg.linear.x = 0.0;                    // 转向时不前进
        vel_msg.angular.z = turn_direction * TURN_SPEED; // 转向角速度
        vel_pub.publish(vel_msg);

        ros::spinOnce(); // 处理IMU回调，更新累计角度
        rate.sleep();
    }

    // 转向完成：停止
    geometry_msgs::Twist stop_msg;
    stop_msg.linear.x = 0.0;
    stop_msg.angular.z = 0.0;
    vel_pub.publish(stop_msg);

    ROS_WARN("=== Turn Done! Actual Angle: %.1f ° ===", fabs(accumulated_angle) * 180 / M_PI);
}

// ===================== 电机使能函数 =====================
void enable_motor() {
    ros::Publisher enable_pub = ros::NodeHandle().advertise<std_msgs::Bool>("/robot/motor/enable", 1);
    std_msgs::Bool enable_msg;
    enable_msg.data = true;
    enable_pub.publish(enable_msg);
    ROS_WARN("=== [IMU Node] Motor Enabled ===");
    ros::Duration(0.5).sleep(); // 等待使能生效
}

// ===================== 主函数（IMU节点核心） =====================
int main(int argc, char **argv) {
    // 1. 初始化ROS节点（禁用默认信号处理）
    ros::init(argc, argv, "w2a_imu_node", ros::init_options::NoSigintHandler);
    ros::NodeHandle nh;

    // 2. 注册Ctrl+C信号处理
    signal(SIGINT, sigint_handler);

    // 3. 初始化发布器/订阅器
    vel_pub = nh.advertise<geometry_msgs::Twist>(CMD_VEL_TOPIC, 10);
    imu_sub = nh.subscribe(IMU_TOPIC, 10, imu_data_callback);

    // 4. 电机使能（可选：如果需要控制小车）
    enable_motor();

    // 5. 打印启动信息
    ROS_INFO("=== W2A IMU Node Started ===");
    ROS_INFO("IMU Topic: %s", IMU_TOPIC.c_str());
    ROS_INFO("Velocity Topic: %s", CMD_VEL_TOPIC.c_str());
    ROS_INFO("Target Turn Angle: 25°");
    ROS_INFO("Press Ctrl+C to exit");

    // 6. 主循环（可扩展：比如接收指令触发转向）
    ros::Rate rate(10); // 10Hz
    while (!g_quit && ros::ok()) {
        ros::spinOnce(); // 处理IMU回调
        rate.sleep();
    }

    // 7. 退出前停止小车
    geometry_msgs::Twist stop_msg;
    stop_msg.linear.x = 0.0;
    stop_msg.angular.z = 0.0;
    vel_pub.publish(stop_msg);

    ROS_INFO("=== [IMU Node] Exited Safely ===");
    return 0;
}
