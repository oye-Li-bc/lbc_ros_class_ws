#include <ros/ros.h>
#include <sensor_msgs/Imu.h>
#include <geometry_msgs/Twist.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <cmath>

// 全局变量
double current_yaw = 0.0;  // 当前偏航角
double target_yaw = 0.0;   // 目标偏航角
ros::Publisher cmd_vel_pub;

// IMU回调：四元数转欧拉角，获取yaw
void imu_callback(const sensor_msgs::Imu::ConstPtr& imu_msg) {
    tf2::Quaternion q(
        imu_msg->orientation.x,
        imu_msg->orientation.y,
        imu_msg->orientation.z,
        imu_msg->orientation.w
    );
    tf2::Matrix3x3 m(q);
    double roll, pitch, yaw;
    m.getRPY(roll, pitch, yaw);
    current_yaw = yaw;

    ROS_INFO("当前偏航角: %.2f rad | %.2f °", 
             current_yaw, current_yaw * 180.0 / M_PI);
}

// 自旋180°主逻辑
void rotate_180_degree() {
    geometry_msgs::Twist cmd_vel;
    target_yaw = current_yaw + M_PI;

    // 角度归一化到 [-π, π]
    if (target_yaw > M_PI)  target_yaw -= 2 * M_PI;
    if (target_yaw < -M_PI) target_yaw += 2 * M_PI;

    ROS_INFO("开始旋转180° | 目标偏航角: %.2f rad", target_yaw);

    ros::Rate rate(10);
    while (ros::ok()) {
        double diff = target_yaw - current_yaw;

        // 处理角度环绕，走最短路径
        if (diff > M_PI)  diff -= 2 * M_PI;
        if (diff < -M_PI) diff += 2 * M_PI;

        // 到达目标（误差 < 2.86°）
        if (fabs(diff) < 0.05) {
            cmd_vel.angular.z = 0.0;
            cmd_vel_pub.publish(cmd_vel);
            ROS_INFO("✅ 180°旋转完成！");
            break;
        }

        // 设置旋转速度
        cmd_vel.angular.z = 0.3 * (diff > 0 ? 1 : -1);
        cmd_vel_pub.publish(cmd_vel);

        ros::spinOnce();
        rate.sleep();
    }
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "imu_rotate_180");
    ros::NodeHandle nh;

    ros::Subscriber imu_sub = nh.subscribe("/imu/data", 10, imu_callback);
    cmd_vel_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 10);

    // 等待IMU数据初始化
    ros::Rate init_rate(10);
    while (ros::ok() && current_yaw == 0.0) {
        ros::spinOnce();
        init_rate.sleep();
    }

    // 执行180°自旋
    rotate_180_degree();

    ros::spin();
    return 0;
}

