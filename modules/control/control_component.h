﻿#pragma once

#include <memory>
#include <string>

#include "cyber/common/macros.h"
#include "cyber/component/timer_component.h"
#include "cyber/cyber.h"
#include "cyber/timer/timer.h"

#include "modules/canbus/proto/chassis.pb.h"
#include "modules/common/adapters/adapter_gflags.h"
#include "modules/control/common/control_gflags.h"
#include "modules/control/proto/control_cmd.pb.h"
#include "modules/control/proto/control_conf.pb.h"
#include "modules/control/proto/pad_msg.pb.h"
#include "modules/drivers/proto/rfid.pb.h"
#include "modules/drivers/proto/wheelangle.pb.h"
#include "modules/drivers/proto/parking.pb.h"

namespace apollo {
namespace control {

using apollo::canbus::Chassis;
using apollo::control::ControlCommand;
using apollo::control::ControlConf;
using apollo::control::PadMessage;
using apollo::drivers::RFID;
using apollo::drivers::WheelAngle;
using apollo::drivers::PARKING;

class ControlComponent final : public apollo::cyber::TimerComponent {
 public:
  ControlComponent();
  std::string Name() const;
  ~ControlComponent();

 private:
  bool Init() override;
  bool Proc() override;
  double PidSpeed();
  double GetSteerTarget(float lat_dev_mgs, double& target_last);
  Chassis chassis_;
  ControlConf control_conf_;
  RFID rfid_front_;
  RFID rfid_rear_;
  PadMessage pad_msg_;
  WheelAngle front_wheel_angle_;
  WheelAngle rear_wheel_angle_;
  bool pad_received_ = false;

  std::shared_ptr<cyber::Reader<apollo::canbus::Chassis>> chassis_reader_;
  std::shared_ptr<cyber::Reader<apollo::drivers::RFID>> rfid_front_reader_;
  std::shared_ptr<cyber::Reader<apollo::drivers::RFID>> rfid_rear_reader_;
  std::shared_ptr<cyber::Writer<ControlCommand>> control_cmd_writer_;
  std::shared_ptr<cyber::Reader<PadMessage>> pad_msg_reader_;
  std::shared_ptr<cyber::Reader<apollo::drivers::WheelAngle>>
      front_wheel_angle_reader_;
  std::shared_ptr<cyber::Reader<apollo::drivers::WheelAngle>>
      rear_wheel_angle_reader_;
  std::shared_ptr<cyber::Reader<apollo::drivers::PARKING>>
      parking_reader_;

  double manual_front_wheel_target = 0;
  double manual_rear_wheel_target = 0;

  std::future<void> async_action_;
  double pid_int = 0;
  double pid_e_pre = 0;

  double front_wheel_target = 0;
  double rear_wheel_target = 0;

  double front_target_last = 0;
  double rear_target_last = 0;

  bool front_wheel_wakeup = false;
  bool rear_wheel_wakeup = false;

  double front_wheel_angle_value = 0.0;
  bool is_front_received = false;

  double rear_wheel_angle_value = 0.0;
  bool is_rear_received = false;

  double parking_value = 0.0;

  bool is_front_destination = false;
  bool is_rear_destination = false;
  bool limit_rear_wheel = true;

  bool limit_front_wheel = true;
};

static const int encoder2wheel_gear_ratio = 125;
static float drivemotor_torque = 0;

// TODO: need calibration
static const float speed_motor_deadzone_calibration = 50;

static const float r_wheel = 0.34;
static const float m_veh = 13000;
constexpr float g = 9.8;
constexpr float f_c = 0.018;
constexpr float i_1 = 3.11;
constexpr float i_0 = 5.857;
constexpr float yita_t = 0.85;

CYBER_REGISTER_COMPONENT(ControlComponent)
}  // namespace control
}  // namespace apollo
