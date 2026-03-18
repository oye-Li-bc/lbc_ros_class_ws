#!/usr/bin/env python3
import rospy
from my_class_pkg.srv import MyServiceMsg

if __name__ == "__main__":
    # Initialize ROS node
    rospy.init_node("python_service_client")
    # Wait for service to start (avoid call before server is ready)
    rospy.wait_for_service("my_service")
    try:
        # Create service client
        client = rospy.ServiceProxy("my_service", MyServiceMsg)
        # Send request (input=2)
        response = client(2)
        rospy.loginfo(f"Response received: output = {response.output}")
    except rospy.ServiceException as e:
        rospy.logerr(f"Service call failed: {e}")

