#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Twist.h>

ros::Publisher cmdVelPub;
ros::Timer avoid_timer;

float firstRange, lastRange;
bool avoiding = false;

void avoidTimerCallback(const ros::TimerEvent&)
{
    // 避障结束
    avoiding = false;
    avoid_timer.stop();
    ROS_WARN("避障完成，恢复正常运动");
}

void scanCallback(const sensor_msgs::LaserScan::ConstPtr &scan)
{
    int num_points = (scan->angle_max - scan->angle_min) / scan->angle_increment;

    firstRange = scan->ranges[num_points / 2];
    lastRange  = scan->ranges[num_points / 2 + 1];
    float meanRange = (firstRange + lastRange) / 2.0;

    ROS_INFO("Mean range: %.2f", meanRange);

    geometry_msgs::Twist cmdVelMsg;

    // 正在避障中：直接发避障速度
    if (avoiding)
    {
        cmdVelMsg.linear.x = -0.15;
        cmdVelMsg.angular.z = 0.5;
    }
    // 触发避障
    else if (meanRange < 0.4)
    {
        ROS_WARN("检测到近距离障碍，开始避障");
        avoiding = true;
        cmdVelMsg.linear.x = -0.15;
        cmdVelMsg.angular.z = 0.5;

        avoid_timer.setPeriod(ros::Duration(1.0)); // 1秒
        avoid_timer.start();
    }
    // 正常前进
    else
    {
        cmdVelMsg.linear.x = 0.3;
        cmdVelMsg.angular.z = 0.0;
    }

    cmdVelPub.publish(cmdVelMsg);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "laser_avoid_node");
    ros::NodeHandle n;

    cmdVelPub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    ros::Subscriber sub = n.subscribe("/scan", 1000, scanCallback);

    avoid_timer = n.createTimer(ros::Duration(1.0), avoidTimerCallback, true);

    ros::spin();
    return 0;
}
