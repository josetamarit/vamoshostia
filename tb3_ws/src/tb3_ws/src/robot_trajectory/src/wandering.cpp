#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <cmath> //M_PI
#include <Eigen/Dense>
#include <vector>
using namespace std::chrono_literals;

std::vector<float> vector;
float min_val;
bool stop=false;

void topic_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
   {
    std::cout << vector[i] << " ";
    

    if (vector[0] < 1){
    	stop = true;
    }

    std::cout << std::endl;
    min_val = eigen_vector.minCoeff();

    std::cout << "Minimum value: " << min_val << std::endl;

    std::cout << vector[0] << std::endl;


  const sensor_msgs::msg::LaserScan::SharedPtr msg;

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

  rclcpp::shutdown();
  return 0;
}