#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <cmath>
#include <memory>
#include <cstdlib>
#include "turtlesim/srv/set_pen.hpp"
#include "turtlesim/srv/teleport_absolute.hpp"
#include <vector>




using namespace std::chrono_literals;
using turtlesim::srv::SetPen;
using turtlesim::srv::TeleportAbsolute;


int main(int argc, char * argv[])
{
 rclcpp::init(argc, argv);
 auto node = rclcpp::Node::make_shared("rings");
 auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);


 node->declare_parameter("radius_length", 1.0);
 geometry_msgs::msg::Twist message;
 rclcpp::WallRate loop_rate(500ms);


 double radius = node->get_parameter("radius_length").get_parameter_value().get<double>();


 double circle= 2* M_PI;


 int iterations = static_cast<int>(circle/0.500);


 RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Ready to set pen.");


  auto client = node->create_client<SetPen>("/turtle1/set_pen");


  auto request = std::make_shared<SetPen::Request>();


  request->width = 5;    
 
  std::vector<int> red = {0, 0, 255, 255, 0};
  std::vector<int> green = {0, 0, 0, 255, 255};
  std::vector<int> blue = {0, 255, 0, 0, 0};


  while (!client->wait_for_service(1s)) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Interrupted while waiting for the service.");
      return 0;
    }
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "service not available, waiting again...");
  }


  auto result = client->async_send_request(request);


  if (rclcpp::spin_until_future_complete(node,result) == rclcpp::FutureReturnCode::SUCCESS)
  {
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Todo bien");
  } else  {
    RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Failed to call service SetPen");
  }




  auto client_tp = node->create_client<TeleportAbsolute>("/turtle1/teleport_absolute");


  auto request_tp = std::make_shared<TeleportAbsolute::Request>();


  request_tp->x = 5.5;
  request_tp->y = 5.5;


  std::vector<double> x = {5.5, (5.5-2*radius), (5.5+2*radius), (5.5-radius), (5.5+radius)};
  std::vector<double> y = {5.5, 5.5, 5.5, (5.5-radius), (5.5-radius)};  
     


  while (!client_tp->wait_for_service(1s)) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Interrupted while waiting for the service.");
      return 0;
    }
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "service not available, waiting again...");
  }


  auto result_tp = client_tp->async_send_request(request_tp);


  // Wait for the result
  if (rclcpp::spin_until_future_complete(node,result_tp) == rclcpp::FutureReturnCode::SUCCESS)
  {
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Todo bien");
  } else  {
    RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Failed to call service SetPen");
  }  




  


  for (int i=0; i<5; i++) {
    request->off = 1;


    request->r = red[i];
    request->g = green[i];
    request->b = blue[i];
    request_tp->x = x[i];
    request_tp->y = y[i];
    result = client->async_send_request(request);
    result_tp = client_tp->async_send_request(request_tp);


    request->off = 0;
    result = client->async_send_request(request);

	}


  for (int i = 0; i < iterations; i++) {
    message.linear.x = 1;
    message.angular.z = 2*M_PI / circle;
    publisher->publish(message);
 


    rclcpp::spin_some(node);
    loop_rate.sleep();
 }


  message.linear.x = 0;
  message.angular.z = 0;
  publisher->publish(message);


 rclcpp::shutdown();
 return 0;
}


