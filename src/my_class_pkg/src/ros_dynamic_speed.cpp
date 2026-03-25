#include <ros/ros.h>
#include <dynamic_reconfigure/server.h>
#include <my_class_pkg/TutorialsConfig.h>
#include <geometry_msgs/Twist.h>

// 全局变量：发布者 + 速度
ros::Publisher g_cmd_pub;
double g_speed = 0.5;

// 动态参数回调
void callback(my_class_pkg::TutorialsConfig &config, uint32_t level)
{
    ROS_INFO("动态参数更新: %.2f", config.double_param);
    g_speed = config.double_param;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "turtle_controller");
    ros::NodeHandle nh;

    // ✅ 【关键】创建发布者（必须写对，全局生效）
    g_cmd_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 10);

    // ✅ 启动动态参数服务
    dynamic_reconfigure::Server<my_class_pkg::TutorialsConfig> server;
    dynamic_reconfigure::Server<my_class_pkg::TutorialsConfig>::CallbackType f;
    f = boost::bind(&callback, _1, _2);
    server.setCallback(f);

    ROS_INFO("✅ 乌龟速度节点启动成功！");

    // ✅ 持续发布速度（核心循环）
    ros::Rate rate(10);
    while(ros::ok())
    {
        geometry_msgs::Twist twist;
        twist.linear.x = g_speed;
        twist.angular.z = 0.0;

        // 持续发布
        g_cmd_pub.publish(twist);

        ros::spinOnce();
        rate.sleep();
    }

    return 0;
}


