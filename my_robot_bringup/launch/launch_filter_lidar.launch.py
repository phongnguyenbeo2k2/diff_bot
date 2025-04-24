from launch import LaunchDescription
from launch.substitutions import PathJoinSubstitution
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    rplidar_node = Node(
            package='rplidar_ros',
            executable='rplidar_composition',
            output='screen',
            parameters=[{
                'serial_port': '/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.4:1.0-port0',
                'frame_id': 'laser_frame',
                'serial_baudrate' : 115200,
                'angle_compensate': True,
                'scan_mode': 'Standard'
            }]
        )
    # filter_lidar = Node(
    #        package="laser_filters",
    #        executable="scan_to_scan_filter_chain",
    #        parameters=[
    #            PathJoinSubstitution([
    #                get_package_share_directory("my_robot_controller"),
    #                "config", "config_param_scan_filter.yaml",
    #            ])],
    #     )
    return LaunchDescription([
        rplidar_node,
        # filter_lidar
    ])