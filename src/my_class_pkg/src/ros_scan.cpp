#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>

void scanCallback(const sensor_msgs::LaserScan::ConstPtr &scan) {
    // 计算激光点总数
    int num_points = (scan->angle_max - scan->angle_min) / scan->angle_increment;
    ROS_INFO_STREAM("Number of points: " << num_points);

    // 第一个点：-π 方向
    float first_range = scan->ranges[0];
    ROS_INFO_STREAM("First range: " << first_range);

    // 最后一个点：π 方向
    float last_range = scan->ranges[num_points - 1];
    ROS_INFO_STREAM("Last range: " << last_range);

    // 中间点：0° 正前方
    float half_range = scan->ranges[num_points/2];
    ROS_INFO_STREAM("Half range: " << half_range );

    // 1/4位置：-π/2 左侧
    float left_range = scan->ranges[num_points/4];
    ROS_INFO_STREAM("Left range: " << left_range );

    // 3/4位置：π/2 右侧
    float right_range = scan->ranges[num_points / 4 * 3];
    ROS_INFO_STREAM("Right range: " << right_range );
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "laser_scan_listener");
    ros::NodeHandle n;
    // 订阅激光雷达话题 /scan
    ros::Subscriber sub = n.subscribe("/scan", 1000, scanCallback);
    ros::spin();
    return 0;
}

