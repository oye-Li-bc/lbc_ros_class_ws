#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <vector>
#include <cmath>
#include <limits>

void scanCallback(const sensor_msgs::LaserScan::ConstPtr& scan)
{
    int N = static_cast<int>(scan->ranges.size());
    if (N == 0) return;

    // 1. Print min range per 10-degree sector
    const int SECTORS = 36;
    std::vector<float> sector_min(SECTORS, std::numeric_limits<float>::infinity());

    for (int i = 0; i < N; ++i) {
        float r = scan->ranges[i];
        if (!std::isfinite(r)) continue;
        float angle_deg = (scan->angle_min + i * scan->angle_increment) * 180.0f / M_PI;
        if (angle_deg < 0) angle_deg += 360.0f;
        int sector = static_cast<int>(angle_deg / 10.0f) % SECTORS;
        if (r < sector_min[sector]) sector_min[sector] = r;
    }

    ROS_INFO("=== min-range per 10-deg sector ===");
    for (int s = 0; s < SECTORS; ++s) {
        float center_deg = s * 10.0f + 5.0f;
        float display = (center_deg > 180.0f) ? center_deg - 360.0f : center_deg;
        if (std::isfinite(sector_min[s])) {
            const char* flag = (sector_min[s] < 0.45f) ? "  <-- PILLAR?" : "";
            ROS_INFO("  %+7.1f deg : %.3f m%s", display, sector_min[s], flag);
        }
    }

    // 2. Auto-detect close clusters
    ROS_INFO("=== Auto spike detection (< 0.45 m) ===");
    bool found = false;
    float cs = 0, ce = 0, cmin = 99;
    bool in_cluster = false;

    for (int i = 0; i < N; ++i) {
        float r = scan->ranges[i];
        float angle = scan->angle_min + i * scan->angle_increment;
        if (std::isfinite(r) && r < 0.45f) {
            if (!in_cluster) { cs = angle; cmin = r; in_cluster = true; }
            ce = angle;
            if (r < cmin) cmin = r;
            found = true;
        } else if (in_cluster) {
            float center = (cs + ce) / 2.0f;
            float hw = (ce - cs) / 2.0f + 0.05f;
            ROS_INFO("  Pillar %.3f rad (%.1f deg) min=%.3fm => mask:[%.2f,%.2f]",
                     center, center*180.0f/M_PI, cmin, center-hw, center+hw);
            in_cluster = false;
        }
    }
    if (in_cluster) {
        float center = (cs + ce) / 2.0f;
        float hw = (ce - cs) / 2.0f + 0.05f;
        ROS_INFO("  Pillar %.3f rad (%.1f deg) min=%.3fm => mask:[%.2f,%.2f]",
                 center, center*180.0f/M_PI, cmin, center-hw, center+hw);
    }
    if (!found)
        ROS_WARN("  No spikes < 0.45m. Is with_angle_filter=false? Bringup restarted?");

    ROS_INFO("========================================");
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "scan_mask_diag_node");
    ros::NodeHandle nh;
    ros::Subscriber sub = nh.subscribe("/scan", 10, scanCallback);
    ROS_INFO("scan_mask_diag: waiting for /scan ...");
    ROS_INFO("Ensure with_angle_filter=false and bringup restarted!");
    ros::spin();
    return 0;
}
