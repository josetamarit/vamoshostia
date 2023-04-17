#include "sensor_msgs/msg/laser_scan.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <cmath> //M_PI
#include <Eigen/Dense>

#include <vector>

using namespace std::chrono_literals;

std::vector<float> vector;
float min_val;

void topic_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
   {

	vector = msg->ranges;

	// Mostrar rango

	for ( int i = 9; i>0; i--) {
    std::cout << vector[i] << " ";
    }
@@ -25,6 +28,12 @@ void topic_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)

    std::cout << std::endl;

    // Mostrar mínimo
    
    Eigen::Map<Eigen::VectorXf> eigen_vector(vector.data(), vector.size());

    min_val = eigen_vector.minCoeff();

    std::cout << "Minimum value: " << min_val << std::endl;
   }