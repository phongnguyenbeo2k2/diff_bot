from launch import LaunchDescription
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory
#TO launch another launch file in this file, we need import
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch_ros.parameter_descriptions import ParameterValue
from launch.substitutions import Command, LaunchConfiguration
from launch.launch_description_sources import PythonLaunchDescriptionSource

def generate_launch_description():

    model_arg = DeclareLaunchArgument(
        name="model",
        default_value=os.path.join(get_package_share_directory("robot_description"),"urdf","mecanumbot.urdf.xacro"),
        description="Absolute path to robot URDF file"
    )

    #create dynamic paramter which rely on argument of launch file. If paramater is static, we don't need to use it.
    robot_description_value = ParameterValue(Command(["xacro ", LaunchConfiguration("model")]),
                                       value_type=str)
    robot_state_publisher = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        parameters=[
            {
                "robot_description": robot_description_value,
                "use_sime_time": "true" 
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

    launch_controller_file_path = os.path.join(get_package_share_directory('robot_controller'),'launch','launch_controller.launch.py')

    #launch controller 
    launch_controller = IncludeLaunchDescription(PythonLaunchDescriptionSource(launch_controller_file_path))

    return LaunchDescription([
        model_arg,
        robot_state_publisher,
        gazebo,
        gazebo_entity,
        launch_controller
    ])