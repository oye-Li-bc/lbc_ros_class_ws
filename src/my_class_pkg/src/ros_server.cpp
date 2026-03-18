#include "ros/ros.h"
#include "my_class_pkg/MyServiceMsg.h"

bool serviceCallback(my_class_pkg::MyServiceMsg::Request &req,
                     my_class_pkg::MyServiceMsg::Response &res) {
    res.output = req.input * 2;
    ROS_INFO("Request received: input = %ld, output = %ld", (long int)req.input, (long int)res.output);
    return true;
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "service_server");
    ros::NodeHandle nh;
    ros::ServiceServer server = nh.advertiseService("my_service", serviceCallback);
    ROS_INFO("Service server started, waiting for requests...");
    ros::spin();
    return 0;
}
