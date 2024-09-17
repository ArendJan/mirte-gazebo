#pragma once
#include <memory>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"

#include "std_msgs/msg/header.hpp"
#include "std_msgs/msg/int32.hpp"

#include "std_srvs/srv/set_bool.hpp"

#include "mirte_msgs/msg/servo_position.hpp"
#include "mirte_msgs/srv/get_servo_range.hpp"
#include "mirte_msgs/srv/set_motor_speed.hpp"
#include "mirte_msgs/srv/set_servo_angle.hpp"
#include "mirte_msgs/srv/set_speed_multiple.hpp"

#include <mirte_telemetrix_cpp/mirte-board.hpp>
#include <mirte_telemetrix_cpp/parsers/p_modules.hpp>

#include <tmx_cpp/tmx.hpp>
#include <tmx_cpp/modules/PCA9685.hpp>

#include <mirte_telemetrix_cpp/modules/base_module.hpp>

class Mirte_modules {
public:
  Mirte_modules(std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<tmx_cpp::TMX> tmx,
                std::shared_ptr<Mirte_Board> board,
                std::shared_ptr<Parser> parser);
  std::shared_ptr<tmx_cpp::TMX> tmx;
  std::shared_ptr<rclcpp::Node> nh;
  std::shared_ptr<Mirte_Board> board;
  std::vector<std::shared_ptr<Mirte_module>> modules;
  std::shared_ptr<tmx_cpp::Modules> module_sys;
  std::shared_ptr<tmx_cpp::Sensors> sensor_sys;
};


class PCA_Motor;
class PCA_Module : public Mirte_module {
public:
  PCA_Module(std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<tmx_cpp::TMX> tmx,
             std::shared_ptr<Mirte_Board> board, std::string name,
             std::shared_ptr<tmx_cpp::Modules> modules,
             std::shared_ptr<PCA_data> pca_data);
  std::shared_ptr<tmx_cpp::PCA9685_module> pca9685;
  std::vector<std::shared_ptr<PCA_Motor>> motors;

  std::shared_ptr<rclcpp::Service<mirte_msgs::srv::SetSpeedMultiple>>
      motor_service;
  bool motor_service_cb(
      const std::shared_ptr<mirte_msgs::srv::SetSpeedMultiple::Request> req,
      std::shared_ptr<mirte_msgs::srv::SetSpeedMultiple::Response> res);
  //   std::vector<std::shared_ptr<PCA_Servo>> servos;
  static std::vector<std::shared_ptr<PCA_Module>>
  get_pca_modules(std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<tmx_cpp::TMX> tmx,
                  std::shared_ptr<Mirte_Board> board,
                  std::shared_ptr<Parser> parser,
                  std::shared_ptr<tmx_cpp::Modules> modules);
};

class PCA_Motor {
public:
  PCA_Motor(std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<tmx_cpp::TMX> tmx,
            std::shared_ptr<Mirte_Board> board,
            std::shared_ptr<PCA_Motor_data> motor_data,
            std::shared_ptr<tmx_cpp::PCA9685_module> pca9685);
  std::shared_ptr<PCA_Motor_data> motor_data;
  std::shared_ptr<tmx_cpp::PCA9685_module> pca9685_mod;
  // Stolen from mirtes-actuators.hpp::motor, but it was too shit to inherit
  // from that one as well.
  void set_speed(
      int speed, bool direct = true,
      std::shared_ptr<std::vector<tmx_cpp::PCA9685_module::PWM_val>> pwm_vals = {});
  rclcpp::Service<mirte_msgs::srv::SetMotorSpeed>::SharedPtr motor_service;
  bool service_callback(
      const std::shared_ptr<mirte_msgs::srv::SetMotorSpeed::Request> req,
      std::shared_ptr<mirte_msgs::srv::SetMotorSpeed::Response> res);
  void motor_callback(const std_msgs::msg::Int32 &msg);
  int last_speed = 0;
  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr ros_client;
};

class Hiwonder_servo;
class Hiwonder_bus_module : public Mirte_module {
public:
  Hiwonder_bus_module(std::shared_ptr<rclcpp::Node> nh,
                      std::shared_ptr<tmx_cpp::TMX> tmx,
                      std::shared_ptr<Mirte_Board> board, std::string name,
                      std::shared_ptr<tmx_cpp::Modules> modules,
                      std::shared_ptr<Hiwonder_bus_data> bus_data);
  std::shared_ptr<tmx_cpp::HiwonderServo_module> bus;
  std::vector<std::shared_ptr<Hiwonder_servo>> servos;
  //   std::vector<std::shared_ptr<PCA_Servo>> servos;

  std::shared_ptr<Hiwonder_bus_data> bus_data;
  static std::vector<std::shared_ptr<Hiwonder_bus_module>>

  get_hiwonder_modules(std::shared_ptr<rclcpp::Node> nh,
                       std::shared_ptr<tmx_cpp::TMX> tmx,
                       std::shared_ptr<Mirte_Board> board,
                       std::shared_ptr<Parser> parser,
                       std::shared_ptr<tmx_cpp::Modules> modules);

  void position_cb(std::vector<tmx_cpp::HiwonderServo_module::Servo_pos>);
  void verify_cb(int, bool);
  void range_cb(int, uint16_t, uint16_t);
  void offset_cb(int, uint16_t);

  // ROS:
  rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr enable_service;
  bool enable_cb(const std::shared_ptr<std_srvs::srv::SetBool::Request> req,
                 std::shared_ptr<std_srvs::srv::SetBool::Response> res);
};

class Hiwonder_servo {
public:
  Hiwonder_servo(std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<tmx_cpp::TMX> tmx,
                 std::shared_ptr<Mirte_Board> board,
                 std::shared_ptr<Hiwonder_servo_data> servo_data,
                 std::shared_ptr<tmx_cpp::HiwonderServo_module> bus);
  std::shared_ptr<Hiwonder_servo_data> servo_data;
  std::shared_ptr<tmx_cpp::HiwonderServo_module> bus_mod;

  // callbacks from the pico
  void position_cb(tmx_cpp::HiwonderServo_module::Servo_pos &pos);

  // ROS:
  //  set_x_servo_enable
  rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr enable_service;
  bool enable_cb(const std::shared_ptr<std_srvs::srv::SetBool::Request> req,
                 std::shared_ptr<std_srvs::srv::SetBool::Response> res);

  // set_x_servo_angle
  rclcpp::Service<mirte_msgs::srv::SetServoAngle>::SharedPtr angle_service;
  bool
  angle_cb(const std::shared_ptr<mirte_msgs::srv::SetServoAngle::Request> req,
           std::shared_ptr<mirte_msgs::srv::SetServoAngle::Response> res);

  // get_x_servo_range
  rclcpp::Service<mirte_msgs::srv::GetServoRange>::SharedPtr range_service;
  bool
  range_cb(const std::shared_ptr<mirte_msgs::srv::GetServoRange::Request> req,
           std::shared_ptr<mirte_msgs::srv::GetServoRange::Response> res);

  // /servos/x/position publisher
  rclcpp::Publisher<mirte_msgs::msg::ServoPosition>::SharedPtr position_pub;

  // angle calcs
  uint16_t calc_angle_out(float angle_in);
  float calc_angle_in(uint16_t angle_out);
};

#include "sensor_msgs/msg/battery_state.hpp"
#include "tmx_cpp/sensors/INA226.hpp"
class INA226_sensor : public Mirte_module {
public:
  INA226_sensor(std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<tmx_cpp::TMX> tmx,
                std::shared_ptr<Mirte_Board> board, std::string name,
                std::shared_ptr<tmx_cpp::Sensors> modules,
                std::shared_ptr<INA226_data> ina_data);
  std::shared_ptr<INA226_data> ina_data;
  std::shared_ptr<tmx_cpp::INA226_module> ina226;

  void data_cb(float voltage, float current);
  void publish();
  float calc_soc(float voltage);
  void integrate_usage(float current);
  void check_soc(float voltage, float current);
  void shutdown_robot();
  void
  shutdown_robot_cb(const std::shared_ptr<std_srvs::srv::SetBool::Request> req,
                    std::shared_ptr<std_srvs::srv::SetBool::Response> res);
  static std::vector<std::shared_ptr<INA226_sensor>>
  get_ina_modules(std::shared_ptr<rclcpp::Node> nh, std::shared_ptr<tmx_cpp::TMX> tmx,
                  std::shared_ptr<Mirte_Board> board,
                  std::shared_ptr<Parser> parser,
                  std::shared_ptr<tmx_cpp::Sensors> sensors);

private:
  float total_used_mAh = 0;
  rclcpp::Time used_time = rclcpp::Time(0, 0);
  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr used_pub;
  rclcpp::Publisher<sensor_msgs::msg::BatteryState>::SharedPtr battery_pub;

  bool enable_turn_off = false;
  bool shutdown_triggered = false;
  rclcpp::Time turn_off_trigger_time = rclcpp::Time(0, 0);
  bool in_power_dip = false;
  rclcpp::Time turn_off_time = rclcpp::Time(0, 0);
  rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr shutdown_service;
};