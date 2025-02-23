#ifndef MECANUMBOT_DRIVER_CONTROLLER
#define MECANUMBOT_DRIVER_CONTROLLER

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>
#include <string>

class MecanumbotController: public rclcpp::Node 
{
public:
    MecanumbotController(const std::string &name);
private:
    void velCallBack(const geometry_msgs::msg::Twist &msg);
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr vel_sub_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr wheel_cmd_pub_;
    double wheel_radius_;
    double lx_;
    double ly_;
};

#endif