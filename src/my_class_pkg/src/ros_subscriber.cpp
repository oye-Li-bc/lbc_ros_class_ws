#include "ros/ros.h"
#include "std_msgs/String.h"

/**
 * 功能：ROS C++订阅者回调函数（文档5.1.2标准版本）
 * 订阅话题：/lbc_topic
 * 接收消息后打印内容
 */
void chatterCallback(const std_msgs::String::ConstPtr& msg)
{
    // 打印接收到的消息
    ROS_INFO("接收到发布者消息: [%s]", msg->data.c_str());
}

int main(int argc, char **argv)
{
    // 初始化ROS节点，节点名：lbc_subscriber_node
    ros::init(argc, argv, "lbc_subscriber_node");
    // 创建节点句柄
    ros::NodeHandle n;

    /**
     * 初始化订阅者
     * 参数1：订阅的话题名 /lbc_topic
     * 参数2：消息队列大小 10
     * 参数3：回调函数名 chatterCallback
     */
    ros::Subscriber sub = n.subscribe("/lbc_topic", 10, chatterCallback);

    /**
     * 循环等待回调函数触发
     * spin()会阻塞直到节点关闭
     */
    ros::spin();

    return 0;
}

