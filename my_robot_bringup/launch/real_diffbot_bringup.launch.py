from launch import LaunchDescription
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory
#TO launch another launch file in this file, we need import
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument, TimerAction
from launch_ros.parameter_descriptions import ParameterValue
from launch.substitutions import Command, LaunchConfiguration
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.actions import RegisterEventHandler
from launch.event_handlers import OnProcessStart

def generate_launch_description():

    model_arg = DeclareLaunchArgument(
        name="model",
        default_value=os.path.join(get_package_share_directory("my_robot_description"),"urdf","my_robot.urdf.xacro"),
        description="Absolute path to robot URDF file"
    )

    #Declare the launch arguments
    declare_use_sim_time_cmd = DeclareLaunchArgument(
        name='use_sim_time',
        default_value='False',
        description='Use simulation (Gazebo) time if true')

    declare_use_ros2_control_cmd = DeclareLaunchArgument(
        name='use_ros2_control',
        default_value='True',
        description='Use ros2_control if true')
    use_sim_time = LaunchConfiguration('use_sim_time')
    use_ros2_control = LaunchConfiguration('use_ros2_control')
    #create dynamic paramter which rely on argument of launch file. If paramater is static, we don't need to use it.
    robot_description_value = ParameterValue(Command(["xacro ", LaunchConfiguration("model"), ' use_ros2_control:=', use_ros2_control, ' sim_mode:=', use_sim_time]),
                                       value_type=str)
    robot_state_publisher = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        parameters=[
            {
                "robot_description": robot_description_value,
                "use_sime_time": use_sim_time
            }
        ]
    )
    controler_yaml_file = os.path.join(get_package_share_directory('my_robot_controller'),'config', 'control_manager.yaml')
    ekf_yaml_file = os.path.join(get_package_share_directory('my_robot_controller'),'config', 'ekf.yaml')
    launch_lidar_and_filter_path = os.path.join(get_package_share_directory('my_robot_bringup'),'launch','launch_filter_lidar.launch.py')   
    #Include gazebo launch file, provided by gazebo_ros package
    controller_manager= Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[ {"robot_description": robot_description_value, 
                    }, controler_yaml_file
                    ],
        output="both",
    )

    #Delay until robot state publisher is runned
    delayed_controller_manager = TimerAction(period=3.0, actions=[controller_manager])
    
    diff_drive_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["diff_cont"],
    )
    #Delay that node until node controller_manager is runned
    delayed_diff_drive_spawner = RegisterEventHandler(
        event_handler=OnProcessStart(
            target_action=controller_manager,
            on_start=[diff_drive_spawner],
        )
    )

    joint_broad_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_broad"],
    )    

    delayed_joint_broad_spawner = RegisterEventHandler(
        event_handler=OnProcessStart(
            target_action=controller_manager,
            on_start=[joint_broad_spawner],
        )
    )
    # start imu_broadcaster to handle data from MPU6050
    # start_imu_broadcaster_cmd = Node(
    #     package="controller_manager",
    #     executable="spawner",
    #     arguments=["imu_broadcaster"],
    # )

    # delayed_imu_broadcaster_spawner = RegisterEventHandler(
    #     event_handler=OnProcessStart(
    #         target_action=controller_manager,
    #         on_start=[start_imu_broadcaster_cmd],
    #     )
    # )
    #start own odom publsiher node 
    start_odom_publisher = Node(
        package="my_robot_controller",
        executable="own_odom_publisher"
    )
    start_trajectory_drawer = Node(
        package="my_robot_controller",
        executable="trajectory_drawer",
        parameters=[{
            'odom_topic_name': '/diffbot/own_odom'
        }]
    )
    #start extended kalman filter node fro mrobot localization package
    # start_robot_localization_cmd = Node(
    #     package="robot_localization",
    #     executable="ekf_node",
    #     name="ekf_filter_node",
    #     output="screen",
    #     parameters=[ekf_yaml_file],
    # )
    #Launch lidar 
    filter_lidar = IncludeLaunchDescription(PythonLaunchDescriptionSource(launch_lidar_and_filter_path))

    return LaunchDescription([
        model_arg,
        declare_use_ros2_control_cmd,
        declare_use_sim_time_cmd,
        robot_state_publisher,
        delayed_controller_manager,
        delayed_diff_drive_spawner,
        delayed_joint_broad_spawner,
        # delayed_imu_broadcaster_spawner,
        start_odom_publisher,
        # start_robot_localization_cmd,
        filter_lidar,
        start_trajectory_drawer
    ])