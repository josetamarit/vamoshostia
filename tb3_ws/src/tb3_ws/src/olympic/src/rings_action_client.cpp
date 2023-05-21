#include <inttypes.h>
#include <memory>
#include "olympic_interfaces/action/rings.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include <iomanip>
#include <sstream>

// We need the action and goal handle classes, the chrono literals, and a pointer to the ROS node
using Rings = 
  olympic_interfaces::action::Rings;

using GoalHandleRings =
  rclcpp_action::ClientGoalHandle<Rings>;

using namespace std::chrono_literals;

rclcpp::Node::SharedPtr g_node = nullptr;

// Assuming the goal was accepted by the server, it will start processing. Any feedback to the client will be handled by the feedback_callback
void feedback_callback(GoalHandleRings::SharedPtr,
  const std::shared_ptr<const Rings::Feedback> feedback)
{
  std::stringstream ss;
  ss << std::setprecision(3) << "Circle n." << feedback->drawing_ring << " at "
     << feedback->ring_angle << " degrees";
  RCLCPP_INFO(
    g_node->get_logger(),
    ss.str().c_str());  // stringstream to string and then to char*
}

// The action client requires 3 things: 
// The templated action type name: Rings
// A ROS 2 node to add the action client to: g_node
// The action name: "rings"
int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  g_node = rclcpp::Node::make_shared("action_client");
  g_node->declare_parameter("radius", 1.0);  // Radius parameter
  double radius = g_node->get_parameter("radius").get_parameter_value().get<double>();
  auto action_client = rclcpp_action::create_client<Rings>(
    g_node, "rings");
  
  if (!action_client->wait_for_action_server(20s)) {
    RCLCPP_ERROR(g_node->get_logger(), 
      "Action server not available after waiting");
    return 1;
  }
  // Populate a goal
  auto goal_msg = Rings::Goal();
  goal_msg.radius = radius;

  // Ask server to achieve the goal
  RCLCPP_INFO(g_node->get_logger(), 
    "Sending goal");
  auto send_goal_options = 
    rclcpp_action::Client<Rings>::SendGoalOptions();
  send_goal_options.feedback_callback = feedback_callback;
  auto goal_handle_future = 
    action_client->async_send_goal(goal_msg, send_goal_options);

  // Wait until it's accepted or rejected
  auto return_code = rclcpp::spin_until_future_complete(g_node,
    goal_handle_future);
  
  // Exit in case of failure or if the goal has been rejected by the server
  if (return_code != rclcpp::FutureReturnCode::SUCCESS)
  {
    RCLCPP_ERROR(g_node->get_logger(), 
      "send goal call failed :(");
    return 1;
  }

  GoalHandleRings::SharedPtr goal_handle = 
    goal_handle_future.get();
  if (!goal_handle) {
    RCLCPP_ERROR(g_node->get_logger(), 
      "Goal was rejected by server");
    return 1;
  }
  
  // Wait for the server to be done with the goal
  auto result_future = 
    action_client->async_get_result(goal_handle);

  RCLCPP_INFO(g_node->get_logger(), "Waiting for result");

  return_code = rclcpp::spin_until_future_complete(g_node, 
    result_future);

  if (return_code != rclcpp::FutureReturnCode::SUCCESS)
  {
    RCLCPP_ERROR(g_node->get_logger(), 
      "get result call failed :(");
    return 1;
  }
  GoalHandleRings::WrappedResult wrapped_result = 
    result_future.get();

  switch (wrapped_result.code) {
    case rclcpp_action::ResultCode::SUCCEEDED:
      break;
    case rclcpp_action::ResultCode::ABORTED:
      RCLCPP_ERROR(g_node->get_logger(), "Goal was aborted");
      return 1;
    case rclcpp_action::ResultCode::CANCELED:
      RCLCPP_ERROR(g_node->get_logger(), "Goal was canceled");
      return 1;
    default:
      RCLCPP_ERROR(g_node->get_logger(), "Unknown result code");
      return 1;
  }
  RCLCPP_INFO(g_node->get_logger(), "Result received");

  action_client.reset();
  g_node.reset();
  rclcpp::shutdown();
  return 0;
}



