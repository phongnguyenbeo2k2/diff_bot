from launch import LaunchDescription
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory
#TO launch another launch file in this file, we need import
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch_ros.parameter_descriptions import ParameterValue
from launch.substitutions import Command, LaunchConfiguration
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.conditions import IfCondition, UnlessCondition

def generate_launch_description():

    model_arg = DeclareLaunchArgument(
        name="model",
        default_value=os.path.join(get_package_share_directory("my_robot_description"),"urdf","my_robot.urdf.xacro"),
        description="Absolute path to robot URDF file"
    )

    # Declare the launch arguments
    declare_use_sim_time_cmd = DeclareLaunchArgument(
        name='use_sim_time',
        default_value='True',
        description='Use simulation (Gazebo) time if true')

    declare_use_ros2_control_cmd = DeclareLaunchArgument(
        name='use_ros2_control',
        default_value='False',
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
    gazebo_launch_file_path = os.path.join(get_package_share_directory('gazebo_ros'),'launch', 'gazebo.launch.py')

    #Include gazebo launch file, provided by gazebo_ros package

    gazebo = IncludeLaunchDescription(PythonLaunchDescriptionSource(gazebo_launch_file_path))

    gazebo_entity = Node(
        package="gazebo_ros",
        executable="spawn_entity.py",
        arguments=['-topic', 'robot_description',
                   '-entity', 'mecanumbot'],
        output='screen'           
    )

    diff_drive_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["diff_cont"],
        condition=IfCondition(LaunchConfiguration('use_ros2_control'))
    )

    joint_broad_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_broad"],
        condition=IfCondition(LaunchConfiguration('use_ros2_control'))
    )    
    # teleop_node = Node(
    #         package='teleop_twist_keyboard',
    #         executable='teleop_twist_keyboard',
    #         name='teleop_twist_keyboard',
    #         output='screen',
    #         prefix='xterm -e',  # Mở trong một terminal mới nếu cần
    #         remappings=[('/cmd_vel', '/diff_cont/cmd_vel_unstamped')],
    #         condition=IfCondition(LaunchConfiguration('use_ros2_control'))
    # )
    # original_teleop_node = Node(
    #         package='teleop_twist_keyboard',
    #         executable='teleop_twist_keyboard',
    #         name='teleop_twist_keyboard',
    #         output='screen',
    #         prefix='xterm -e',  # Mở trong một terminal mới nếu cần
    #         condition=UnlessCondition(LaunchConfiguration('use_ros2_control'))
    # )


    rviz2 = Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            output='screen',
            arguments=['-d', os.path.join(get_package_share_directory('my_robot_description'),'rviz',"my_robot_config.rviz")]  # Đường dẫn tới file cấu hình RViz (nếu có)
    )
    return LaunchDescription([
        model_arg,
        declare_use_sim_time_cmd,
        declare_use_ros2_control_cmd,
        robot_state_publisher,
        gazebo,
        gazebo_entity,
        diff_drive_spawner,
        joint_broad_spawner,
        # teleop_node,
        # original_teleop_node,
        rviz2
    ])