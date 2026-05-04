#include "tf2_ros/transform_listener.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.h"
#include "geometry_msgs/TransformStamped.h"
#include "geometry_msgs/PointStamped.h"
#include "upros_message/ArmPosition.h"
#include "upros_message/TagCommand.h"
#include "std_srvs/Empty.h"
#include <ros/ros.h>

int target_tag = 0;

void sleep(double second)
{
    ros::Duration(second).sleep();
}

void cmd_callback(const upros_message::TagCommand::ConstPtr &msg)
{
    target_tag = msg->target;
    ROS_INFO("✅ 收到抓取命令，目标Tag: %d", target_tag);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "voice_grab");
    ros::NodeHandle nh;

    ros::Subscriber cmd_sub = nh.subscribe("/voice_control", 10, cmd_callback);
    ros::Rate rate(10);

    ROS_INFO("⏳ 等待抓取命令...");
    while (target_tag == 0) {
        rate.sleep();
        ros::spinOnce();
    }

    std::string tag_link;
    if(target_tag == 1) {
        tag_link = "tag_1";
    } else if (target_tag == 2) {
        tag_link = "tag_2";
    } else {
        ROS_ERROR("❌ 不支持的Tag编号: %d", target_tag);
        return -1;
    }

    tf2_ros::Buffer buffer;
    tf2_ros::TransformListener listener(buffer);
    ROS_INFO("🔄 正在获取 %s 相对于 arm_base_link 的坐标...", tag_link.c_str());

    geometry_msgs::TransformStamped tfs_1;
    try {
        tfs_1 = buffer.lookupTransform("arm_base_link", tag_link, ros::Time(0), ros::Duration(5));
        ROS_INFO("✅ 成功获取到坐标变换！");
    } catch (tf2::TransformException &ex) {
        ROS_ERROR("❌ 获取坐标变换失败: %s", ex.what());
        return -1;
    }

    // --- 打印原始坐标 ---
    ROS_INFO("📊 原始坐标数据:");
    ROS_INFO("   x: %.3f m", tfs_1.transform.translation.x);
    ROS_INFO("   y: %.3f m", tfs_1.transform.translation.y);
    ROS_INFO("   z: %.3f m", tfs_1.transform.translation.z);

    // --- 计算目标坐标（和你之前的逻辑保持一致）---
    int x = -int(tfs_1.transform.translation.y * 1000);
    int y = int(tfs_1.transform.translation.x * 1000) + 30;
    int z = int(tfs_1.transform.translation.z * 1000 + 40);

    ROS_INFO("🎯 转换后的目标坐标 (mm): x=%d, y=%d, z=%d", x, y, z);

    // --- 初始化服务客户端 ---
    ros::ServiceClient arm_move_open_client =
    nh.serviceClient<upros_message::ArmPosition>("/upros_arm_control/arm_pos_service_open");
    ros::ServiceClient arm_grab_client =
    nh.serviceClient<std_srvs::Empty>("/upros_arm_control/grab_service");
    ros::ServiceClient arm_zero_client =
    nh.serviceClient<std_srvs::Empty>("/upros_arm_control/zero_service");

    // --- 【重点】检查服务是否存在 ---
    if (!arm_move_open_client.exists()) {
        ROS_ERROR("❌ 致命错误：移动服务 /upros_arm_control/arm_pos_service_open 不存在！");
        return -1;
    }
    if (!arm_grab_client.exists()) {
        ROS_ERROR("❌ 致命错误：抓取服务 /upros_arm_control/grab_service 不存在！");
        return -1;
    }

    // --- 1. 移动到目标位置 ---
    ROS_INFO("🚀 正在调用移动服务...");
    upros_message::ArmPosition move_srv;
    move_srv.request.x = x;
    move_srv.request.y = y;
    move_srv.request.z = z;

    bool move_success = arm_move_open_client.call(move_srv);
    if (move_success) {
        ROS_INFO("✅ 移动服务调用成功！机械臂正在移动...");
    } else {
        ROS_ERROR("❌ 移动服务调用失败！机械臂无法移动！");
        return -1;
    }
    sleep(5.0); // 给机械臂足够时间移动到位

    // --- 2. 执行抓取 ---
    ROS_INFO("🤏 正在调用抓取服务...");
    std_srvs::Empty empty_srv;
    bool grab_success = arm_grab_client.call(empty_srv);
    if (grab_success) {
        ROS_INFO("✅ 抓取服务调用成功！");
    } else {
        ROS_ERROR("❌ 抓取服务调用失败！");
    }
    sleep(5.0);

    // --- 3. 回到零位 ---
    ROS_INFO("🏠 正在调用回零服务...");
    bool zero_success = arm_zero_client.call(empty_srv);
    if (zero_success) {
        ROS_INFO("✅ 回零服务调用成功！");
    } else {
        ROS_ERROR("❌ 回零服务调用失败！");
    }
    sleep(5.0);

    ROS_INFO("🎉 抓取流程全部完成！");
    ros::spin();
    return 0;
}

