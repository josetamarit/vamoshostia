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

rclcpp_action::GoalResponse handle_goal(
  const rclcpp_action::GoalUUID & uuid, 
  std::shared_ptr<const Rings::Goal> goal)
{
  RCLCPP_INFO(rclcpp::get_logger("server"), 
    "Got goal request with radius %.2f", goal->radius);
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

void handle_accepted(const std::shared_ptr<GoalHandleRings> goal_handle){
  std::thread{execute, goal_handle}.detach();
}


void execute(const std::shared_ptr<GoalHandleRings> goal_handle) {
  // Action related
  RCLCPP_INFO(rclcpp::get_logger("server"), 
    "Executing goal");
  const auto goal = goal_handle->get_goal();
  auto feedback = std::make_shared<Rings::Feedback>();
  auto result = std::make_shared<Rings::Result>();
  auto & ring_number = feedback->drawing_ring; // Reference to drawing_ring(ptr)
  auto & ring_angle = feedback->ring_angle; // Reference to ring_angle(ptr)
  
  // Ring related
  float radius = goal->radius;
  auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
  geometry_msgs::msg::Twist message;

  double time_for_circle = 2 * M_PI * radius / 1.0; // time to complete a circle
  int num_iterations = static_cast<int>(time_for_circle / 0.01); // number of iterations to complete a circle
  rclcpp::WallRate loop_rate(10ms);

  // iteration values
  std::vector<int> r = {0, 0, 255, 255, 0};
  std::vector<int> g = {0, 0, 0, 255, 255};
  std::vector<int> b = {0, 255, 0, 0, 0};   

  std::vector<double> x = {5.5, ((5.5-2*radius)-(0.2*radius)), ((5.5+2*radius)+(0.2*radius)), (5.5-radius-(0.2*radius)), (5.5+radius)+(0.2*radius)};
  std::vector<double> y = {5.5, 5.5, 5.5, (5.5-radius), (5.5-radius)};   
 
  //set_pen client
  rclcpp::Client<SetPen>::SharedPtr client =
	node->create_client<SetPen>("/turtle1/set_pen");
	auto request = std::make_shared<SetPen::Request>();
 
  // Initialize TeleportAbsolute service
	rclcpp::Client<TeleportAbsolute>::SharedPtr client_tp =
	node->create_client<TeleportAbsolute>("/turtle1/teleport_absolute");
	auto request_tp= std::make_shared<TeleportAbsolute::Request>();
   // tp middle
  request_tp->x = 5.5; 
  request_tp->y = 5.5;

  auto result_tp = client_tp->async_send_request(request_tp);
 
  for (int i=0; i<5; i++) {
    ring_number = i+1;
    ring_angle = 0;
    request->off = 1;
    request->width= 5;
    request->r = r[i]; 
    request->g = g[i]; 
    request->b = b[i];
    request_tp->x = x[i]; 
    request_tp->y = y[i];
    auto result = client->async_send_request(request);
    result_tp = client_tp->async_send_request(request_tp);

    request->off = 0;
    result = client->async_send_request(request); 

    for (int j = 0; j < num_iterations; j++) { // do circle
      message.linear.x = 1;
      message.angular.z = 2 * M_PI / time_for_circle;
      publisher->publish(message);
      loop_rate.sleep();

      // Feedback
      ring_number = i+1;
			goal_handle->publish_feedback(feedback);
    	RCLCPP_INFO(rclcpp::get_logger("server"), "Publish Feedback");
      if (j == num_iterations/4) {
        ring_angle = 90;
				goal_handle->publish_feedback(feedback);
    		RCLCPP_INFO(rclcpp::get_logger("server"), 
       		"Publish Feedback");
      }else if (j == num_iterations/2) {
        ring_angle = 180;
				goal_handle->publish_feedback(feedback);
    		RCLCPP_INFO(rclcpp::get_logger("server"), 
       		"Publish Feedback");
      }else if (j == 3*num_iterations/4) {
        ring_angle = 270;
				goal_handle->publish_feedback(feedback);
    		RCLCPP_INFO(rclcpp::get_logger("server"), 
       		"Publish Feedback");
      } 
    }
    // send zero velocity to topic
    message.linear.x = 0;
    message.angular.z = 0;
    publisher->publish(message);
    loop_rate.sleep();
  }

    if (rclcpp::ok()) { // When completed, if ROS still running:
    result->rings_completed = ring_number;
    goal_handle->succeed(result);
    RCLCPP_INFO(rclcpp::get_logger("server"), 
      "Goal Succeeded");
    rclcpp::shutdown();
  }
}

int main(int argc, char ** argv){
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
