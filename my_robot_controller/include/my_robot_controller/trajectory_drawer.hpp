#ifndef TRAJECTORY_DRAWER_HPP
#define TRAJECTORY_DRAWER_HPP

#include <nav_msgs/msg/odometry.hpp>
#include <rclcpp/rclcpp.hpp>
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "nav_msgs/msg/path.hpp"

class TrajectoryDrawer : public rclcpp::Node
{
public:
    TrajectoryDrawer(const std::string &name);
private:
    void odomCallback(const nav_msgs::msg::Odometry &msg);
    /*1 subsriber with odom topic*/
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
    /*1 publisher topic /diffbot/trajectory*/
    rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr trajectory_pub_;
    nav_msgs::msg::Path path_msg_;
    std::string odom_topic_name_;
};
#endif 