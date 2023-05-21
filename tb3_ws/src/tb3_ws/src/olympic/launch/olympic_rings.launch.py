from launch import LaunchDescription
from launch_ros.actions import Node
import launch.actions

def generate_launch_description():
	return LaunchDescription([
		Node(
			package='turtlesim',
			executable='turtlesim_node'
		),
		Node(
			package='olympic',
      executable='rings',
      parameters=[
        {"radius": 1.7},
      ]
		),
		launch.actions.ExecuteProcess(
      cmd=['ros2', 'param', 'set', '/turtlesim', 'background_r', '255'],
      output='screen'
    ),
    launch.actions.ExecuteProcess(
      cmd=['ros2', 'param', 'set', '/turtlesim', 'background_g', '255'],
      output='screen'
    ),
    launch.actions.ExecuteProcess(
      cmd=['ros2', 'param', 'set', '/turtlesim', 'background_b', '255'],
      output='screen'
    )
	])
	
