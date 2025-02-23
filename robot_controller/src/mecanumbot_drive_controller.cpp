#include "robot_controller/mecanumbot_drive_controller.hpp"

MecanumbotController::MecanumbotController(const std::string &name) : Node(name)
{
    declare_parameter("lx", 0.22525);
    declare_parameter("ly", 0.2);
    declare_parameter("wheel_radius", 0.050);
    lx_ = get_parameter("lx").as_double();
    ly_ = get_parameter("ly").as_double();
    wheel_radius_ = get_parameter("wheel_radius").as_double();
    RCLCPP_INFO_STREAM(this->get_logger(), "Using lx_: " << lx_);
    RCLCPP_INFO_STREAM(this->get_logger(), "Using ly_: " << ly_);
    wheel_cmd_pub_ = create_publisher<std_msgs::msg::Float64MultiArray>("/simple_velocity_controller/commands", 10);
    vel_sub_ = create_subscription<geometry_msgs::msg::Twist>("/cmd_vel", 10, std::bind(&MecanumbotController::velCallBack, this, std::placeholders::_1));
    RCLCPP_INFO_STREAM(this->get_logger(), "Mecanumbot controller start!");
}
void MecanumbotController::velCallBack(const geometry_msgs::msg::Twist &msg)
{
    (void)msg;
    RCLCPP_INFO_STREAM(this->get_logger(), "The velCallBackfunction is calledd!!!");
    float v_x = msg.linear.x;
    float v_y = msg.linear.y;
    float w_z = msg.angular.z;

    float w_fl = 0, w_fr = 0, w_rl = 0, w_rr = 0;
    /*Inverse kinematic mecanumbot*/
    w_fl = (v_x - v_y - (lx_ + ly_)*w_z)/wheel_radius_;
    w_fr = (v_x + v_y + (lx_ + ly_)*w_z)/wheel_radius_;
    w_rl = (v_x + v_y - (lx_ + ly_)*w_z)/wheel_radius_;
    w_rr = (v_x - v_y + (lx_ + ly_)*w_z)/wheel_radius_;
    RCLCPP_INFO_STREAM(this->get_logger(), "w_fl: " << w_fl << " ,w_fr: " << w_fr << " ,w_rl: " << w_rl << " ,w_rr: " << w_rr);
    std_msgs::msg::Float64MultiArray wheel_speed_msg;
    wheel_speed_msg.data.push_back(w_fl);
    wheel_speed_msg.data.push_back(w_fr);
    wheel_speed_msg.data.push_back(w_rl);
    wheel_speed_msg.data.push_back(w_rr);
    wheel_cmd_pub_->publish(wheel_speed_msg);
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MecanumbotController>("mecanumbot_controller");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}