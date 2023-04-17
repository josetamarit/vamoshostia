#include <iostream>
#include <string.h>
#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "example_interfaces/msg/bool.hpp"

enum State {
  S1,
  S2,
  S3
};

State state = S1;

using namespace std::chrono_literals;

bool front_obstacle = false, left_obstacle = false, right_obstacle = false;

void callback_front(const example_interfaces::msg::Bool::SharedPtr msg)
{
	front_obstacle = msg->data; 
}
void callback_left(const example_interfaces::msg::Bool::SharedPtr msg)
{
	left_obstacle = msg->data; 
}
void callback_right(const example_interfaces::msg::Bool::SharedPtr msg)
{
	right_obstacle = msg->data; 
}

   
int main(int argc, char * argv[]) 	
{

  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("avoidance");
  auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
  auto subs_front = node->create_subscription<example_interfaces::msg::Bool>("/front/obstacle", 10, callback_front);
  auto subs_left = node->create_subscription<example_interfaces::msg::Bool>("/left/obstacle", 10, callback_left);
  auto subs_right = node->create_subscription<example_interfaces::msg::Bool>("/right/obstacle", 10, callback_right);
  geometry_msgs::msg::Twist message;
  rclcpp::WallRate loop_rate(50ms);
 
    
  while (rclcpp::ok()) {
	
	switch (state) {
    	case S1:
      		if (front_obstacle) {
        	state = S2;
     		}
      		break;
    	case S2:
      		if (left_obstacle & front_obstacle) {
        		state = S3;
      		} else if ( !front_obstacle ) {
        		state = S1;
      		}
      		break;
      	case S3:
      		if (!front_obstacle) {
        		state = S1;
      		}
      		break;
  	}

  	if (state == S1) {
    	message.linear.x = 0.5;
    	message.angular.z = 0;
  	} else if (state == S2) {
    	message.linear.x = 0;
    	message.angular.z = 0.5;
  	} else if (state == S3) {
    	message.linear.x = 0;
    	message.angular.z = -0.5;
  	}

  	publisher->publish(message);
  	rclcpp::spin_some(node);
  	loop_rate.sleep(); 
	}
  
 
  rclcpp::shutdown();
  return 0;
}