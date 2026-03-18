#!/usr/bin/env python3
import rospy
from my_class_pkg.srv import MyServiceMsg, MyServiceMsgResponse

# Service callback function: handle request and return response
def handle_service_request(req):
    rospy.loginfo(f"Request received: input = {req.input}")
    return MyServiceMsgResponse(req.input * 2)

if __name__ == "__main__":
    # Initialize ROS node
    rospy.init_node("python_service_server")
    # Advertise service (service name: my_service)
    rospy.Service("my_service", MyServiceMsg, handle_service_request)
    rospy.loginfo("Python Service Server started, waiting for requests...")
    # Keep server running
    rospy.spin()

