#include "ros/ros.h"
#include "my_class_pkg/MyServiceMsg.h"

int main(int argc, char **argv) {
    ros::init(argc, argv, "service_client");
    ros::NodeHandle nh;
    ros::ServiceClient client = nh.serviceClient<my_class_pkg::MyServiceMsg>("my_service");
    
    my_class_pkg::MyServiceMsg srv;
    srv.request.input = 42;
    
    if (client.call(srv)) {
        ROS_INFO("Response received: output = %ld", (long int)srv.response.output);
    } else {
        ROS_ERROR("Service call failed!");
        return 1;
    }
    return 0;
}
