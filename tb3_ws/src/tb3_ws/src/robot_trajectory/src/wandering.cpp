#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <cmath> //M_PI
#include <Eigen/Dense>
#include <vector>
using namespace std::chrono_literals;
std::vector<float> vector;
float min_val0;
float min_val350;
bool stop = false;
float angular;
void topic_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
   {
	
	vector = msg->ranges;
	
	// Mostrar rango
	for ( int i = 9; i>0; i--) {
    std::cout << vector[i] << " ";
    }
    
    for ( int i = 359; i>350; i--) {
    std::cout << vector[i] << " ";
    }

    if (vector[0] < 1){
    	stop = true;
    } else {
    	stop = false;
    }


    std::cout << std::endl;

    // Mostrar mínimo
    Eigen::Map<Eigen::VectorXf> eigen_vector0(vector.data(), 10);
    Eigen::Map<Eigen::VectorXf> eigen_vector350(vector.data()+350, 10);
    
    min_val0 = eigen_vector0.minCoeff(); 
    min_val350 = eigen_vector350.minCoeff(); 
    
    
   }
    
int main(int argc, char * argv[]) 	
{
  
  rclcpp::init(argc, argv);
  //subscriber
  auto node = rclcpp::Node::make_shared("subscriber");
  auto subscription = node->create_subscription<sensor_msgs::msg::LaserScan>("scan", 10, topic_callback);
  
  // publisher
  auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
  geometry_msgs::msg::Twist message;
  rclcpp::WallRate loop_rate(10ms);
  
  const sensor_msgs::msg::LaserScan::SharedPtr msg;
  
  //std::cout << vector[0] << std::endl;
  
  while (rclcpp::ok() && stop==false) // move forward 
  {
   	message.linear.x = 0.5;
  	message.angular.z = 0;
    publisher->publish(message);
    rclcpp::spin_some(node);
    loop_rate.sleep();
    
  }
  
  message.linear.x = 0;
  publisher->publish(message);
  rclcpp::spin_some(node);
  loop_rate.sleep();

  if ( min_val0 > min_val350) {
   		angular = 0.5;
   	} else {
   		angular = -0.5;
  	}


  while (rclcpp::ok() && stop==true) // girar izquierda 
  {  
   	message.linear.x = 0;
  	message.angular.z = angular;
    publisher->publish(message);
    rclcpp::spin_some(node);
    loop_rate.sleep();

  }
  message.angular.z = 0;
  publisher->publish(message);
  rclcpp::spin_some(node);
  loop_rate.sleep();


  rclcpp::shutdown();
  return 0;
}