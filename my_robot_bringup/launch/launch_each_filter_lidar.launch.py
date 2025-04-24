from launch import LaunchDescription
from launch.substitutions import PathJoinSubstitution
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    filter_lidar = Node(
           package="laser_filters",
           executable="scan_to_scan_filter_chain",
           parameters=[
               PathJoinSubstitution([
                   get_package_share_directory("my_robot_controller"),
                   "config", "config_param_scan_filter.yaml",
               ])],
        )
    return LaunchDescription([
        filter_lidar
    ])