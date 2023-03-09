#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include <cmath>


using namespace std::chrono_literals;

double xp;
double yp;
double xo;
double yo;
double angle;
double initialxp;
double initialyp;
double initialangle;

void topic_callback(const nav_msgs::msg::Odometry::SharedPtr msg){
	if (initialxp == 0 && initialyp == 0 && initialangle == 0) {
        	initialxp = msg->pose.pose.position.x;
        	initialyp = msg->pose.pose.position.y;
        	initialangle = std::atan2(msg->pose.pose.orientation.y, msg->pose.pose.orientation.x);
    }
    	xp = msg->pose.pose.position.x;
	yp = msg->pose.pose.position.y;
	xo = msg->pose.pose.orientation.x;
	yo = msg->pose.pose.orientation.y;
	angle = std::atan2(yo, xo);
	std::cout << "Pos x: " << xp << std::endl;
	std::cout << "Pos y: " << yp << std::endl;
	std::cout << "Angle in radians: " << angle << std::endl;
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

