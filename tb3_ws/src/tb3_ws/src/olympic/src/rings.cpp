#include <chrono>
#include "rclcpp/rclcpp.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/srv/set_pen.hpp"
#include "turtlesim/srv/teleport_absolute.hpp"
#include "turtlesim/srv/teleport_relative.hpp"

using namespace std::chrono_literals;
using turtlesim::srv::SetPen;
using turtlesim::srv::TeleportAbsolute;
using turtlesim::srv::TeleportRelative;


int main(int argc, char * argv[]) {
  rclcpp::init(argc, argv);
  
  // Node, publisher and message
	std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("rings");
	auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
	geometry_msgs::msg::Twist message;
	
	// Data
	node->declare_parameter("radius", 1.0);
	double radius = node->get_parameter("radius").get_parameter_value().get<double>();
	float v_angular = 1.2,
			 	v_linear  = radius * v_angular;
	int j, iterations = 2*M_PI / (v_angular * 0.1);
				 
	rclcpp::WallRate loop_rate(100ms);
	
	// Iteration values
	int r[5] = {0, 0, 223, 244, 0};
	int g[5] = {129, 0, 0, 195, 159};
	int b[5] = {200, 0, 36, 0, 61};
	
	double desplazamiento_x[5] = {-2.166667, 0, 2.166667, -1.166667, 1.166667};
	double desplazamiento_y[5] = {0, 0, 0, -1, -1};
	
	// Initialize SetPen service
	rclcpp::Client<SetPen>::SharedPtr client_set_pen =
		node->create_client<SetPen>("/turtle1/set_pen");
	auto request_set_pen = std::make_shared<SetPen::Request>();
	auto result_set_pen = client_set_pen->async_send_request(request_set_pen);
	
	// Initialize TeleportAbsolute service
	rclcpp::Client<TeleportAbsolute>::SharedPtr client_teleport_absolute =
		node->create_client<TeleportAbsolute>("/turtle1/teleport_absolute");
	auto request_teleport_absolute = std::make_shared<TeleportAbsolute::Request>();
	auto result_teleport_absolute = client_teleport_absolute->async_send_request(request_teleport_absolute);
	
	
	for (int i = 0; i < 5; i++) {
		// Change colors and disable pen
		request_set_pen->r = r[i];
		request_set_pen->g = g[i];
		request_set_pen->b = b[i];
		request_set_pen->width = 5;
		request_set_pen->off = 1;
		
		while (!client_set_pen->wait_for_service(1s)) {
		  if (!rclcpp::ok()) {
		    RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), 
		     "Circle %i: Interrupted while waiting for the SetPen service.", i+1);
		    return 0;
		 	}
		 	RCLCPP_INFO(rclcpp::get_logger("rclcpp"), 
		   "Circle %i: SetPen service not available, waiting again...", i+1);
  	}
		result_set_pen = client_set_pen->async_send_request(request_set_pen);
		
		if (rclcpp::spin_until_future_complete(node, 
       result_set_pen) ==	rclcpp::FutureReturnCode::SUCCESS)
		{
		  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), 
		   "Circle %i: Succesfully called SetPen service", i+1);
		} else {
		  RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), 
		   "Circle %i: Failed to call service SetPen", i+1);
		}
		
		// Circle absolute position
		request_teleport_absolute = std::make_shared<TeleportAbsolute::Request>();
		request_teleport_absolute->x = 5.544445 + desplazamiento_x[i] * radius;
		request_teleport_absolute->y = 5.544445 + desplazamiento_y[i] * radius;
		request_teleport_absolute->theta = 0;
		
		while (!client_teleport_absolute->wait_for_service(1s)) {
		  if (!rclcpp::ok()) {
		    RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), 
		     "Circle %i: Interrupted while waiting for the TeleportAbsolute service.", i+1);
		    return 0;
		 	}
		 	RCLCPP_INFO(rclcpp::get_logger("rclcpp"), 
		   "Circle %i: TeleportAbsolute service not available, waiting again...", i+1);
  	}
		result_teleport_absolute = client_teleport_absolute->async_send_request(request_teleport_absolute);
		
		if (rclcpp::spin_until_future_complete(node, 
       result_teleport_absolute) ==	rclcpp::FutureReturnCode::SUCCESS)
		{
		  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), 
		   "Circle %i: Succesfully called TeleportAbsolute service", i+1);
		} else {
		  RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), 
		   "Circle %i: Failed to call service TeleportAbsolute", i+1);
		}
		
		// Turn pen back on
		request_set_pen->off = 0;
		result_set_pen = client_set_pen->async_send_request(request_set_pen);
		
		loop_rate.sleep();
		// Draw the circle
		j = 0;
		while (rclcpp::ok() && (j <= iterations)) {
			message.linear.x = v_linear;
			message.angular.z = v_angular;
			publisher->publish(message);
			
			j++;
			rclcpp::spin_some(node);
			loop_rate.sleep();
		}
		message.linear.x = 0.0;
		message.angular.z = 0.0;
		publisher->publish(message);
		rclcpp::spin_some(node);
		loop_rate.sleep();
	}
	
	rclcpp::shutdown();
	return 0;
}

