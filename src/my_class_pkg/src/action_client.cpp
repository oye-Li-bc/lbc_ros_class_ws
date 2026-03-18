#include "ros/ros.h"
#include "actionlib/client/simple_action_client.h"
#include "my_class_pkg/MyActionMsgAction.h"

// 定义动作客户端类型
typedef actionlib::SimpleActionClient<my_class_pkg::MyActionMsgAction> Client;

// 进度反馈回调函数
void feedbackCallback(const my_class_pkg::MyActionMsgFeedbackConstPtr& feedback) {
    ROS_INFO("Received feedback: current = %d, progress = %.2f", 
             feedback->current, feedback->progress);
}

// 完成回调函数
void doneCallback(const actionlib::SimpleClientGoalState& state,
                  const my_class_pkg::MyActionMsgResultConstPtr& result) {
    if (state == actionlib::SimpleClientGoalState::SUCCEEDED) {
        ROS_INFO("Action finished! Success: %s", result->success ? "true" : "false");
    } else {
        ROS_WARN("Action failed! State: %s", state.toString().c_str());
    }
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "cpp_action_client");
    // 创建动作客户端（服务名：my_action）
    Client client("my_action", true);

    ROS_INFO("Waiting for action server to start...");
    client.waitForServer(); // 等待服务器启动
    ROS_INFO("Action server connected!");

    // 发送目标值：计数到10
    my_class_pkg::MyActionMsgGoal goal;
    goal.target = 10;
    client.sendGoal(goal, &doneCallback, Client::SimpleActiveCallback(), &feedbackCallback);

    // 等待动作完成
    client.waitForResult();
    return 0;
}
