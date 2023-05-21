#include <inttypes.h>
#include <memory>
#include <cmath>
#include "olympic_interfaces/action/rings.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/srv/set_pen.hpp"
#include "turtlesim/srv/teleport_absolute.hpp"

using namespace std::chrono_literals;
using turtlesim::srv::SetPen;
using turtlesim::srv::TeleportAbsolute;

using Rings = 
  olympic_interfaces::action::Rings;

using GoalHandleRings = 
  rclcpp_action::ServerGoalHandle<Rings>;

rclcpp::Node::SharedPtr node = nullptr;

// Callback for handling new goals. This implementation accepts all goals
rclcpp_action::GoalResponse handle_goal(
  const rclcpp_action::GoalUUID & uuid, 
  std::shared_ptr<const Rings::Goal> goal)
{
  RCLCPP_INFO(rclcpp::get_logger("server"), 
    "Got goal request for a circle with radius %.*f", 2, goal->radius); // 2 decimal places
  (void)uuid;
  return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
}

// Callback for dealing with cancellation. This implementation just tells the client it accepted the cancellation
rclcpp_action::CancelResponse handle_cancel(
  const std::shared_ptr<GoalHandleRings> goal_handle)
{
  RCLCPP_INFO(rclcpp::get_logger("server"), 
    "Got request to cancel goal");
  (void)goal_handle;
  return rclcpp_action::CancelResponse::ACCEPT;
}

// This last set of callbacks accepts a new goal and starts processing it.
void execute(const std::shared_ptr<GoalHandleRings>);

void handle_accepted(
  const std::shared_ptr<GoalHandleRings> goal_handle)
{
  std::thread{execute, goal_handle}.detach();
}

void execute(
  const std::shared_ptr<GoalHandleRings> goal_handle)
{
  // Action related
  RCLCPP_INFO(rclcpp::get_logger("server"), 
    "Executing goal");
  const auto goal = goal_handle->get_goal();
  auto feedback = std::make_shared<Rings::Feedback>();
  auto result = std::make_shared<Rings::Result>();
  auto & ring_number = feedback->drawing_ring;  // Reference to drawing_ring
  auto & ring_angle = feedback->ring_angle;     // and ring_angle
  
  // Ring related
  float radius = goal->radius;  // Get the goal from the client
  auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
  geometry_msgs::msg::Twist message;
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
	
	// Initialize TeleportAbsolute service
	rclcpp::Client<TeleportAbsolute>::SharedPtr client_teleport_absolute =
		node->create_client<TeleportAbsolute>("/turtle1/teleport_absolute");
	auto request_teleport_absolute = std::make_shared<TeleportAbsolute::Request>();
  
  // Begin the loop
  for (int i = 0; i < 5; i++) {
    if (goal_handle->is_canceling()) {
      goal_handle->canceled(result);
      RCLCPP_INFO(rclcpp::get_logger("server"), 
      "Goal Canceled");
      return;
    }
    // Change colors and disable pen
		request_set_pen->r = r[i];
		request_set_pen->g = g[i];
		request_set_pen->b = b[i];
		request_set_pen->width = 5;
		request_set_pen->off = 1;
    
    auto result_set_pen = client_set_pen->async_send_request(request_set_pen);
    
    // Circle absolute position
		request_teleport_absolute = std::make_shared<TeleportAbsolute::Request>();
		request_teleport_absolute->x = 5.544445 + desplazamiento_x[i] * radius;
		request_teleport_absolute->y = 5.544445 + desplazamiento_y[i] * radius;
		request_teleport_absolute->theta = 0;

    auto result_teleport_absolute = client_teleport_absolute->async_send_request(request_teleport_absolute);

    // Publish feedback (ring and angle we are currently at)
    ring_number = i+1;
    ring_angle = 0;
    
    goal_handle->publish_feedback(feedback);
    RCLCPP_INFO(rclcpp::get_logger("server"), 
       "Publish Feedback");
    
    // Turn pen back on
		request_set_pen->off = 0;
		result_set_pen = client_set_pen->async_send_request(request_set_pen);
		
    // Draw the circle
		j = 0;
		while (rclcpp::ok() && (j <= iterations)) {
			if (j == iterations/4) {  // 90 degrees
				ring_angle = 90;
				goal_handle->publish_feedback(feedback);
    		RCLCPP_INFO(rclcpp::get_logger("server"), 
       		"Publish Feedback");
			} else if (j == iterations/2) {  // 180 degrees
				ring_angle = 180;
				goal_handle->publish_feedback(feedback);
    		RCLCPP_INFO(rclcpp::get_logger("server"), 
       		"Publish Feedback");
			} else if (j == 3*iterations/4) {  // 270 degrees
				ring_angle = 270;
				goal_handle->publish_feedback(feedback);
    		RCLCPP_INFO(rclcpp::get_logger("server"), 
       		"Publish Feedback");
			}
			message.linear.x = v_linear;
			message.angular.z = v_angular;
			publisher->publish(message);
			
			j++;
			loop_rate.sleep();
		}
		message.linear.x = 0.0;
		message.angular.z = 0.0;
		publisher->publish(message);
		loop_rate.sleep();
  }
  
  if (rclcpp::ok()) {  // When completed, if ROS is still running:
    result->rings_completed = ring_number;
    goal_handle->succeed(result);
    RCLCPP_INFO(rclcpp::get_logger("server"), 
      "All five rings completed");
  }
}


// Main function
int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  node = rclcpp::Node::make_shared("action_server");
  auto action_server = 
    rclcpp_action::create_server<Rings>(node,
      "rings",
      handle_goal,
      handle_cancel,
      handle_accepted);
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
