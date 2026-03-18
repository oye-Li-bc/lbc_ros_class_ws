#include "ros/ros.h"
#include "std_msgs/Int16MultiArray.h"
#include "geometry_msgs/Twist.h"
#include <cmath>

// 全局变量
ros::Publisher vel_pub;
bool is_avoiding = false;          // 避障中标记（防止重复执行）
bool collision_detected = false;   // 单次碰撞标记（防止高频回调重复触发）
ros::Time last_collision_time;     // 最后一次碰撞时间（防抖）

// 精准参数配置（核心！按物理量控制，不是时间）
const float FORWARD_SPEED = 0.2;    // 正常向前速度 (m/s)
const float BACKWARD_SPEED = -0.2;  // 后退速度 (m/s)
const float TURN_SPEED = 0.5;       // 转弯角速度 (rad/s)
const float TARGET_BACK_DIST = 0.2; // 目标后退距离 (m) → 20cm
const float TARGET_TURN_ANGLE = M_PI / 2; // 目标转弯角度 (rad) → 90°

// 发布速度指令
void publish_vel(float linear_x, float angular_z) {
    geometry_msgs::Twist vel_msg;
    vel_msg.linear.x = linear_x;
    vel_msg.angular.z = angular_z;
    vel_pub.publish(vel_msg);
}

// 精准后退：按距离（20cm）控制，不是时间
void precise_backward() {
    ROS_INFO("Start precise backward: target distance = %.2f m", TARGET_BACK_DIST);
    ros::Rate rate(50); // 50Hz高频控制
    float current_dist = 0.0;
    ros::Time start_time = ros::Time::now();

    while (current_dist < TARGET_BACK_DIST && ros::ok()) {
        publish_vel(BACKWARD_SPEED, 0.0);
        // 计算已后退距离：距离 = 速度 × 时间
        float dt = (ros::Time::now() - start_time).toSec();
        current_dist = fabs(BACKWARD_SPEED) * dt;
        rate.sleep();
    }
    // 后退到位，停止
    publish_vel(0.0, 0.0);
    ROS_INFO("Backward finished: actual distance = %.2f m", current_dist);
}

// 精准转弯：按角度（90°）控制，不是时间
void precise_turn(int turn_dir) {
    ROS_INFO("Start precise turn: target angle = %.2f° (%.2f rad)", 
             TARGET_TURN_ANGLE * 180 / M_PI, TARGET_TURN_ANGLE);
    ros::Rate rate(50);
    float current_angle = 0.0;
    ros::Time start_time = ros::Time::now();

    while (current_angle < TARGET_TURN_ANGLE && ros::ok()) {
        publish_vel(0.0, turn_dir * TURN_SPEED);
        // 计算已转弯角度：角度 = 角速度 × 时间
        float dt = (ros::Time::now() - start_time).toSec();
        current_angle = fabs(TURN_SPEED) * dt;
        rate.sleep();
    }
    // 转弯到位，停止
    publish_vel(0.0, 0.0);
    ROS_INFO("Turn finished: actual angle = %.2f° (%.2f rad)", 
             current_angle * 180 / M_PI, current_angle);
}

// 单次避障流程：停止→精准后退20cm→精准转90°→恢复向前
void do_avoidance(int turn_dir) {
    is_avoiding = true;
    collision_detected = false; // 重置碰撞标记
    ROS_WARN("=== Start single avoidance action ===");
    ROS_WARN("Turn direction: %s (90°)", (turn_dir == 1) ? "RIGHT" : "LEFT");

    // 1. 紧急停止
    publish_vel(0.0, 0.0);
    ros::Duration(0.2).sleep();

    // 2. 精准后退20cm
    precise_backward();

    // 3. 精准转90°（1=右转，-1=左转）
    precise_turn(turn_dir);

    // 4. 恢复向前，解除避障标记
    publish_vel(FORWARD_SPEED, 0.0);
    is_avoiding = false;
    ROS_WARN("=== Avoidance action finished ===");
}

// 碰撞传感器回调（防抖+单次触发）
void bumpCallback(const std_msgs::Int16MultiArray::ConstPtr& msg) {
    int left_front = msg->data[0];
    int front = msg->data[1];
    int right_front = msg->data[2];
    bool has_collision = (left_front == 1 || front == 1 || right_front == 1);

    // 打印状态
    ROS_INFO("Bump Sensor: left=%d, front=%d, right=%d | Collision: %s",
             left_front, front, right_front, has_collision ? "YES" : "NO");

    // 核心防抖逻辑：
    // 1. 避障中忽略所有碰撞
    // 2. 无碰撞时重置标记，保持向前
    // 3. 有碰撞但未触发过 → 仅执行1次避障
    if (is_avoiding) return;
    
    if (!has_collision) {
        collision_detected = false;
        publish_vel(FORWARD_SPEED, 0.0);
        return;
    }

    // 单次碰撞仅触发1次避障
    if (!collision_detected) {
        collision_detected = true;
        last_collision_time = ros::Time::now();
        // 左前/正前撞→左转90°(-1)，右前撞→右转90°(1)
        int turn_dir = (left_front == 1 || front == 1) ? -1 : 1;
        do_avoidance(turn_dir);
    }
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "precise_bump_avoidance");
    ros::NodeHandle nh;

    // 订阅传感器话题（降低队列大小，减少回调堆积）
    ros::Subscriber sub = nh.subscribe("/robot/bump_sensor", 1, bumpCallback);
    vel_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 1);

    ROS_INFO("Precise Bump Avoidance Node Started!");
    ROS_INFO("Config: Back 20cm, Turn 90°, Single action per collision");
    publish_vel(FORWARD_SPEED, 0.0); // 初始向前

    ros::spin();
    return 0;
}
