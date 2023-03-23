#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

using namespace std::chrono_literals;


std::vector<float>vector;

void topic_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg){
  vector= msg->ranges;
  for(int i=9;i>0; i--){
   std::cout<<vector[i]<<" ";
  }
  
  for(int i=359;i>350; i--){
   std::cout<<vector[i]<<" ";
  }
  
  std::cout<<std::endl;
  
}

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("wandering");
  auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
  auto subscriber= node->create_subscription<sensor_msgs::msg::LaserScan>("scan", 10, topic_callback);
  geometry_msgs::msg::Twist message;
  rclcpp::WallRate loop_rate(500ms);

  while (rclcpp::ok()) {
    message.linear.x=0.0;
    publisher->publish(message);
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }
  rclcpp::shutdown();
  return 0;
}
