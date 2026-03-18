#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>

/**
 * LBC ROS实验 - C++发布者节点
 * 发布话题：/lbc_topic
 * 消息类型：std_msgs/String
 */
int main(int argc, char **argv)
{
    // 初始化ROS节点
    ros::init(argc, argv, "lbc_publisher_node");
    ros::NodeHandle n;

    // 创建发布者，话题/lbc_topic，队列大小10
    ros::Publisher chatter_pub = n.advertise<std_msgs::String>("/lbc_topic", 10);
    
    // 设置发布频率：10Hz
    ros::Rate loop_rate(10);

    int count = 0;
    while (ros::ok())
    {
        // 构造消息内容
        std_msgs::String msg;
        std::stringstream ss;
        ss << "LBC ROS实验 - 发布消息: " << count;
        msg.data = ss.str();

        // 打印日志+发布消息
        ROS_INFO("%s", msg.data.c_str());
        chatter_pub.publish(msg);

        ros::spinOnce();
        loop_rate.sleep();
        ++count;
    }

    return 0;
}
