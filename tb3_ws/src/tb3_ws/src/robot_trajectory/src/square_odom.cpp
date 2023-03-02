#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include <cmath>


using namespace std::chrono_literals;

double x;
double y;

void topic_callback(const nav_msgs::msg::Odometry::SharedPtr msg){
	x = msg->pose.pose.position.x;
	y = msg->pose.pose.position.y;
	std::cout << "Pos x: " << x << std::endl;
	std::cout << "Pos y: " << y << std::endl;
}

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("subscriber");
  auto subscription = node->create_subscription<nav_msgs::msg::Odometry>("odom", 10, topic_callback);
  
  rclcpp::WallRate loop_rate(10ms); 
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}

