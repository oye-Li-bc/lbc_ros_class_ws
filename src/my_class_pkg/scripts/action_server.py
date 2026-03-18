#!/usr/bin/env python3
import rospy
import actionlib
from my_class_pkg.msg import MyActionMsgAction, MyActionMsgFeedback, MyActionMsgResult

class CountActionServer:
    def __init__(self):
        # 创建动作服务器（服务名：my_action）
        self.server = actionlib.SimpleActionServer(
            "my_action", MyActionMsgAction, self.execute, False
        )
        self.server.start()
        rospy.loginfo("Python Action Server started, waiting for goal...")

    def execute(self, goal):
        rate = rospy.Rate(1) # 1Hz（每秒计数1次）
        target = goal.target
        rospy.loginfo(f"Action server started: counting up to {target}")

        # 修复核心错误：判断逻辑写反了！
        # 原错误：if self.server.is_preempt_requested() or not rospy.is_shutdown():
        # 正确逻辑：只有当「客户端取消请求」OR「ROS关闭」时，才终止
        for i in range(target + 1):
            if self.server.is_preempt_requested() or rospy.is_shutdown():
                self.server.set_preempted()
                return

            # 发布进度反馈（核心：逐秒返回计数进度）
            feedback = MyActionMsgFeedback()
            feedback.current = i
            feedback.progress = float(i) / target if target != 0 else 1.0
            self.server.publish_feedback(feedback)
            rospy.loginfo(f"Progress: current = {i}, progress = {feedback.progress:.2f}")
            rate.sleep()

        # 执行完成，返回成功结果
        result = MyActionMsgResult()
        result.success = True
        self.server.set_succeeded(result)
        rospy.loginfo("Action finished successfully!")

if __name__ == "__main__":
    rospy.init_node("python_action_server")
    server = CountActionServer()
    rospy.spin()

