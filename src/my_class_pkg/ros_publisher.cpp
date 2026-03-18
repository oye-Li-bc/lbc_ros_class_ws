#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>

/**
 * 功能：ROS C++发布者示例（文档5.1.1标准版本）
 * 发布话题：/lbc_topic
 * 消息类型：std_msgs/String
 */
int main(int argc, char **argv)
{
    // 初始化ROS节点，节点名：lbc_publisher_node
    ros::init(argc, argv, "lbc_publisher_node");
    // 创建节点句柄
    ros::NodeHandle n;

    /**
     * 初始化发布者
     * 参数1：发布的话题名 /lbc_topic
     * 参数2：消息队列大小 10
     */
    ros::Publisher chatter_pub = n.advertise<std_msgs::String>("/lbc_topic", 10);

    // 设置发布频率：10Hz
    ros::Rate loop_rate(10);

    int count = 0;
    // ROS节点正常运行时循环
    while (ros::ok())
    {
        // 创建String类型消息对象
        std_msgs::String msg;
        std::stringstream ss;
        ss << "LBC ROS实验 - 发布消息: " << count;
        msg.data = ss.str();

        // 打印日志（可选，方便调试）
        ROS_INFO("%s", msg.data.c_str());

        // 发布消息
        chatter_pub.publish(msg);

        // 处理ROS回调（此处无订阅，可省略，但保留规范）
        ros::spinOnce();

        // 按照设定频率休眠
        loop_rate.sleep();
        ++count;
    }

    return 0;
}

