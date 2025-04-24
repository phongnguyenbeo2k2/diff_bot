from launch import LaunchDescription
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='slam_toolbox',
            executable='async_slam_toolbox_node',  # hoặc "async_slam_toolbox_node"
            name='slam_toolbox',
            output='screen',
            parameters=[os.path.join(get_package_share_directory('my_robot_controller'),'config', 'mapper_params_online_async.yaml')],
            remappings=[
                ('scan', '/scan'),    # hoặc topic bạn dùng
                ('odom', '/diffbot/own_odom')     # từ encoder
            ]
        )
    ])