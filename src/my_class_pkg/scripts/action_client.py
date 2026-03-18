#!/usr/bin/env python3
import rospy
import actionlib
from my_class_pkg.msg import MyActionMsgAction, MyActionMsgGoal

# 进度反馈回调
def feedback_cb(feedback):
    rospy.loginfo(f"Received feedback: current = {feedback.current}, progress = {feedback.progress:.2f}")

if __name__ == "__main__":
    rospy.init_node("python_action_client")
    # 创建动作客户端
    client = actionlib.SimpleActionClient("my_action", MyActionMsgAction)

    rospy.loginfo("Waiting for action server...")
    client.wait_for_server()
    rospy.loginfo("Action server connected!")

    # 发送目标值：计数到5
    goal = MyActionMsgGoal()
    goal.target = 5
    client.send_goal(goal, feedback_cb=feedback_cb)

    # 等待动作完成
    client.wait_for_result()
    rospy.loginfo(f"Action finished! Success: {client.get_result().success}")

