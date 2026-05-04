#!/usr/bin/env python3
from openai import OpenAI
import rospy
from std_msgs.msg import String

# 你的API Key
api_key = "sk-rIoTtLQ53k78ODYeuNb8L6pmaKUxFeoEkhtQ8OvLzTAVeP4P"
base_url = "https://api.moonshot.cn/v1"

class LLMNode:
    def __init__(self):
        self.client = OpenAI(api_key=api_key, base_url=base_url)
        self.model = "moonshot-v1-8k"

        rospy.init_node("llm_voice_chat")
        
        # ==============================================
        # 🔥 修复点：强制订阅 /speech/result，加打印
        # ==============================================
        self.sub = rospy.Subscriber("/speech/result", String, self.callback)
        self.talk_pub = rospy.Publisher("/talk", String, queue_size=1)

        rospy.loginfo("==================================================")
        rospy.loginfo("✅ 大模型节点已启动！等待语音输入...")
        rospy.loginfo("✅ 已订阅话题：/speech/result")
        rospy.loginfo("==================================================")

    # 🔥 强制回调函数
    def callback(self, msg):
        text = msg.data.strip()
        rospy.loginfo(f"[已收到] 语音识别内容：{text}")  # <-- 只要这里打印，就一定能进模型

        if not text:
            return

        try:
            rospy.loginfo("[处理中] 正在请求大模型...")
            response = self.client.chat.completions.create(
                model=self.model,
                messages=[
                    {"role": "system", "content": "简短回答，中文。"},
                    {"role": "user", "content": text}
                ],
                temperature=0.1,
            )
            answer = response.choices[0].message.content.strip()
            rospy.loginfo(f"[回答] {answer}")

            # 发送语音播报
            self.talk_pub.publish(answer)

        except Exception as e:
            rospy.logerr(f"[错误] {e}")
            self.talk_pub.publish("我没听懂，请再说一遍。")

if __name__ == "__main__":
    try:
        node = LLMNode()
        rospy.spin()
    except KeyboardInterrupt:
        rospy.loginfo("程序退出")

