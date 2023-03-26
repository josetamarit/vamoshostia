#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <cmath> //M_PI

using namespace std::chrono_literals;

double xp; 
double yp; 
double orientation_x; 
double orientation_y;
double orientation_z; 
double orientation_w;
double angle;
double initial_xp;
double initial_yp;
double initial_angle;
double distance;
double siny_cosp;
double cospy_cosp;
 

void topic_callback(const nav_msgs::msg::Odometry::SharedPtr msg)
{
	if (initial_xp == 0 && initial_yp == 0 && initial_angle == 0) {
        initial_xp = msg->pose.pose.position.x;
        initial_yp = msg->pose.pose.position.y;
        orientation_x = msg->pose.pose.orientation.x;
    	orientation_y = msg->pose.pose.orientation.y;
    	orientation_z = msg->pose.pose.orientation.z;
    	orientation_w = msg->pose.pose.orientation.w;
        siny_cosp = 2* (orientation_w*orientation_z + orientation_x*orientation_y);
        cospy_cosp = 1 - 2*(orientation_y*orientation_y + orientation_z*orientation_z);
        initial_angle = atan2(siny_cosp, cospy_cosp);
    }
	xp = msg->pose.pose.position.x;
	yp = msg->pose.pose.position.y;
	orientation_x = msg->pose.pose.orientation.x;
    orientation_y = msg->pose.pose.orientation.y;
    orientation_z = msg->pose.pose.orientation.z;
    orientation_w = msg->pose.pose.orientation.w;
    siny_cosp = 2* (orientation_w*orientation_z + orientation_x*orientation_y);
    cospy_cosp = 1 - 2*(orientation_y*orientation_y + orientation_z*orientation_z);
    angle = atan2(siny_cosp, cospy_cosp);
    if (angle < 0) {
    	angle += 2 * M_PI;
    }
    
  
	distance = std::sqrt(std::pow(xp - initial_xp, 2) + std::pow(yp - initial_yp, 2));
	std::cout << "Initial x: " << initial_xp << std::endl;
	std::cout << "Initial y: " << initial_yp << std::endl;
	std::cout << "Initial angle: " << initial_angle << std::endl;
	std::cout << "Pos x: " << xp << std::endl;
	std::cout << "Pos y: " << yp << std::endl;
	std::cout << "Angle in radians: " << angle << std::endl;
	std::cout << "distance: " << distance << std::endl;
	std::cout << "----------" << std::endl;
}

int main(int argc, char * argv[]) 	
{
  
  rclcpp::init(argc, argv);
  //subscriber
  auto node = rclcpp::Node::make_shared("subscriber");
  auto subscription = node->create_subscription<nav_msgs::msg::Odometry>("odom", 10, topic_callback);
  
  // publisher
  auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
  geometry_msgs::msg::Twist message;
  rclcpp::WallRate loop_rate(10ms);
  node->declare_parameter("linear_speed", 0.2);
  node->declare_parameter("angular_speed", 0.2);
  node->declare_parameter("square_length", 1.0);
  double linear_speed = node->get_parameter("linear_speed").get_parameter_value().get<double>();
  double angular_speed = node->get_parameter("angular_speed").get_parameter_value().get<double>();
  double square_length = node->get_parameter("square_length").get_parameter_value().get<double>();
  
  const nav_msgs::msg::Odometry::SharedPtr msg;
  
  
  for ( int i = 1; i<=4; i++ ) {
  
  
  while (rclcpp::ok() && (distance<square_length)) // move forward 
  {
   	message.linear.x = linear_speed;
  	message.angular.z = 0;
    publisher->publish(message);
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }
 
  // send zero velocity to topic
  
  
  message.linear.x = 0;
  publisher->publish(message);
  rclcpp::spin_some(node);
  loop_rate.sleep();
  initial_xp = xp;
  initial_yp = yp;
  loop_rate.sleep();
  
  
  double target_angle = M_PI_2 + initial_angle;
 
  if (target_angle > 2*M_PI) { // Por unas céntesimas puede que el robot se quede girando en bucle
  	target_angle = 6.28
  	;
  }
  std::cout << "target angle: " << target_angle << std::endl;
  
  
  while (rclcpp::ok() && (angle < (target_angle) )) // turn 90 
  {
   std::cout << target_angle << std::endl;
   message.linear.x = 0;
   if ( (target_angle - angle) > 0.25 ) {
   	message.angular.z = angular_speed;
   } else {
   		message.angular.z = 0.1;
   }
   publisher->publish(message);
   rclcpp::spin_some(node);
   loop_rate.sleep();
  }
  message.angular.z = 0;
  publisher->publish(message);
  initial_angle = angle;
  loop_rate.sleep(); 
  }
  
  rclcpp::shutdown();
  return 0;
}

