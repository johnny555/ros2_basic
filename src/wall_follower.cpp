#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

using std::placeholders::_1;

class WallFollower : public rclcpp::Node {
public:
  WallFollower() : Node("wall_follower") {
    cmd_vel_pub =
        this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
    scan_sub = this->create_subscription<sensor_msgs::msg::LaserScan>(
        "/scan", 10, std::bind(&WallFollower::follow_wall, this, _1));
  };

private:
  void follow_wall(const sensor_msgs::msg::LaserScan::SharedPtr msg) {
    geometry_msgs::msg::Twist cmd;
    cmd.linear.x = 0.1;

    auto ranges = msg->ranges;
    float right_range = ranges[270];
    float forward_range = ranges[180];

    // Simple wall follow logic.

    if (right_range > 0.3) {
      cmd.angular.z = -0.1;
    } else {
      if (right_range < 0.2) {
        cmd.angular.z = 0.1;
      }
    }

    // emergency steer left
    if (forward_range < 0.5) {
      cmd.angular.z = 0.3;
    }

    cmd_vel_pub->publish(cmd);
  }

  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub;
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub;
};

// standard main function.
int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<WallFollower>());
  rclcpp::shutdown();
  return 0;
}