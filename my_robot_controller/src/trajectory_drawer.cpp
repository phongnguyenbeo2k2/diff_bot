#include "my_robot_controller/trajectory_drawer.hpp"

TrajectoryDrawer::TrajectoryDrawer(const std::string &name): Node(name)
{
    declare_parameter("odom_topic_name", "/odom");
    odom_topic_name_ = get_parameter("odom_topic_name").as_string();
    /*Create subscription for odom topic */
    odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
        odom_topic_name_, 10,
        std::bind(&TrajectoryDrawer::odomCallback, this, std::placeholders::_1));
    /*Cretae publisher topic /diffbot/trajectory */
    trajectory_pub_ = this->create_publisher<nav_msgs::msg::Path>("/diffbot/trajectory", 10);
    // Khởi tạo header cho path
    path_msg_.header.frame_id = "odom";  // hoặc "map" nếu bạn dùng SLAM
}

void TrajectoryDrawer::odomCallback(const nav_msgs::msg::Odometry &msg)
{
    geometry_msgs::msg::PoseStamped pose_stamped;
    pose_stamped.header = msg.header;
    pose_stamped.pose = msg.pose.pose;

    path_msg_.header.stamp = this->now();  // cập nhật thời gian hiện tại
    path_msg_.poses.push_back(pose_stamped);

    trajectory_pub_->publish(path_msg_);
}

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<TrajectoryDrawer>("trajectory_drawer_node");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}