#include "ros/ros.h"
#include "actionlib/server/simple_action_server.h"
#include "my_class_pkg/MyActionMsgAction.h"

// 定义动作服务器类型
typedef actionlib::SimpleActionServer<my_class_pkg::MyActionMsgAction> Server;

// 动作回调函数：处理目标请求
void execute(const my_class_pkg::MyActionMsgGoalConstPtr& goal, Server* as) {
    ros::Rate rate(1); // 1Hz（每秒执行一次）
    my_class_pkg::MyActionMsgFeedback feedback; // 进度反馈
    my_class_pkg::MyActionMsgResult result;     // 最终结果

    int target = goal->target;
    ROS_INFO("Action server started: counting up to %d", target);

    // 模拟计数过程，实时返回进度
    for (int i = 0; i <= target; i++) {
        // 检查客户端是否取消请求
        if (as->isPreemptRequested() || !ros::ok()) {
            as->setPreempted();
            return;
        }

        // 设置进度反馈
        feedback.current = i;
        feedback.progress = (float)i / target;
        as->publishFeedback(feedback);
        ROS_INFO("Progress: current = %d, progress = %.2f", i, feedback.progress);

        rate.sleep();
    }

    // 执行完成，返回结果
    result.success = true;
    as->setSucceeded(result);
    ROS_INFO("Action finished successfully!");
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "cpp_action_server");
    ros::NodeHandle nh;

    // 创建动作服务器（服务名：my_action）
    Server server(nh, "my_action", boost::bind(&execute, _1, &server), false);
    server.start();
    ROS_INFO("C++ Action Server started, waiting for goal...");

    ros::spin();
    return 0;
}
