#include <ros/ros.h>
#include <my_class_pkg/MyMessage.h>

int main(int argc, char** argv)
{
    ros::init(argc, argv, "my_message_publisher");
    ros::NodeHandle nh;
    ros::Publisher pub = nh.advertise<my_class_pkg::MyMessage>("/my_msg_topic", 10);
    ros::Rate rate(1);
    int key = 0;
    while (ros::ok())
    {
        my_class_pkg::MyMessage msg;
        msg.key = key;
        msg.value = "Hello from C++ publisher, key = " + std::to_string(key);
        ROS_INFO("Publishing: key=%d, value='%s'", msg.key, msg.value.c_str());
        pub.publish(msg);
        ++key;
        rate.sleep();
    }
    return 0;
}
