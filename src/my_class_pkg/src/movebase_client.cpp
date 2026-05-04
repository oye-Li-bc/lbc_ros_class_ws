#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf2/LinearMath/Quaternion.h>
#include <geometry_msgs/Quaternion.h>
#include <cmath>

using namespace std;

// Action client for move_base
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char **argv)
{
    // Initialize ROS node
    ros::init(argc, argv, "send_goals_node");

    // Create move_base action client
    MoveBaseClient ac("move_base", true);
    // Wait for move_base action server
    ac.waitForServer();

    move_base_msgs::MoveBaseGoal goal1, goal2, goal3;
    tf2::Quaternion quat;

    // --------------------------
    // Goal 1: Set your position
    // --------------------------
    quat.setRPY(0, 0, 0);
    goal1.target_pose.pose.position.x = 1.65;
    goal1.target_pose.pose.position.y = 0.47;
    goal1.target_pose.pose.orientation.z = quat.z();
    goal1.target_pose.pose.orientation.w = quat.w();
    goal1.target_pose.header.frame_id = "map";
    goal1.target_pose.header.stamp = ros::Time::now();

    ac.sendGoal(goal1);
    ROS_INFO("Sending Goal 1");
    ac.waitForResult();

    if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        ROS_INFO("Reached Goal 1 successfully!");
    else
        ROS_WARN("Failed to reach Goal 1");

    // --------------------------
    // Goal 2: Set your position
    // --------------------------
    quat.setRPY(0, 0, 0);
    goal2.target_pose.pose.position.x = 1.25;
    goal2.target_pose.pose.position.y = 1.75;
    goal2.target_pose.pose.orientation.z = quat.z();
    goal2.target_pose.pose.orientation.w = quat.w();
    goal2.target_pose.header.frame_id = "map";
    goal2.target_pose.header.stamp = ros::Time::now();

    ac.sendGoal(goal2);
    ROS_INFO("Sending Goal 2");
    ac.waitForResult();

    if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        ROS_INFO("Reached Goal 2 successfully!");
    else
        ROS_WARN("Failed to reach Goal 2");

    // --------------------------
    // Return to origin (0,0)
    // --------------------------
    goal3.target_pose.pose.position.x = 0.05;
    goal3.target_pose.pose.position.y = 0.05;
    goal3.target_pose.pose.orientation.z = 0.0;
    goal3.target_pose.pose.orientation.w = 1.0;
    goal3.target_pose.header.frame_id = "map";
    goal3.target_pose.header.stamp = ros::Time::now();

    ac.sendGoal(goal3);
    ROS_INFO("Returning to origin");
    ac.waitForResult();

    if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        ROS_INFO("Returned to origin successfully!");
    else
        ROS_WARN("Failed to return to origin");

    return 0;
}

