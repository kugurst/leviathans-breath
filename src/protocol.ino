#include "protocol.h"

#include <cmath>
#include <cstdint>
#include <cstring>

#include "config.h"

namespace LB {

std::array<uint8_t, HID_BUF_SIZE> Protocol::rx_hid_buf_;
std::array<uint8_t, HID_BUF_SIZE> Protocol::tx_hid_buf_;
microsDelay Protocol::check_delay_;

void Protocol::print_command_(Command command) {
#ifndef DISABLE_SERIAL
  switch (command) {
  case Command::SET_CURVE:
    break;
  case Command::GET_CURVE:
    Serial.println(F("GET CURVE Received"));
    break;
  case Command::GET_ALL_RPM:
    Serial.println(F("SEND ALL RPM Received"));
    break;
  case Command::GET_ALL_TEMPERATURE:
    Serial.println(F("SEND ALL TEMPERATURE Received"));
  default:
    break;
  }
#endif
}

void clear_buffer(std::array<uint8_t, HID_BUF_SIZE> &buf) {
  memset(buf.data(), 0, sizeof(uint8_t) * buf.size());
}

bool send_buffer(std::array<uint8_t, HID_BUF_SIZE> &buf) {
  if (RawHID.send(buf.data(), HID_TIMEOUT) != RAWHID_TX_SIZE) {
#ifndef DISABLE_SERIAL
    Serial.println(F("Failed to send buffer"));
    return false;
#endif
  }
  return true;
}

void Protocol::init() { check_delay_.start(CHECK_DELAY_US); }

void Protocol::process_command_(Command command,
                                std::array<uint8_t, HID_BUF_SIZE> &extra_data,
                                uint8_t extra_data_offset) {
  CurveCommandParameters curve_command;
  FanParameters fan_command;
  LEDParameters led_command;

  switch (command) {
  case Command::ECHO:
    echo_(extra_data, extra_data_offset);
    break;
  case Command::GET_CURVE:
    memcpy(&curve_command, extra_data.data() + 1,
           sizeof(CurveCommandParameters));
    get_curve_(curve_command);
    break;
  case Command::SET_CURVE:
    memcpy(&curve_command, extra_data.data() + 1,
           sizeof(CurveCommandParameters));
    set_curve_(curve_command);
    FileManager::save_config();
    break;
  case Command::GET_ALL_RPM:
    send_all_rpm_();
    break;
  case Command::GET_ALL_TEMPERATURE:
    send_all_temperatures_();
    break;
  case Command::GET_ALL_FAN_PARAMETERS:
    get_all_fan_parameters_();
    break;
  case Command::SET_FAN_PARAMETERS:
    memcpy(&fan_command, extra_data.data() + 1, sizeof(FanParameters));
    set_fan_parameters_(fan_command);
    FileManager::save_config();
    break;
  case Command::GET_ALL_LED_PARAMETERS:
    get_all_led_parameters_();
    break;
  case Command::SET_LED_PARAMETERS:
    memcpy(&led_command, extra_data.data() + 1, sizeof(LEDParameters));
    set_led_parameters_(led_command);
    FileManager::save_config();
    break;
  default:
#ifndef DISABLE_SERIAL
    Serial.print(F("Unknown command: "));
    Serial.println((uint32_t)command);
#endif
    break;
  }
}

bool Protocol::echo_(std::array<uint8_t, HID_BUF_SIZE>& extra_data, uint8_t extra_data_offset) {
  clear_buffer(tx_hid_buf_);

  tx_hid_buf_[0] = (uint8_t)Command::ECHO;
  memcpy(tx_hid_buf_.data() + 1, extra_data.data() + extra_data_offset, HID_BUF_SIZE - extra_data_offset);
  if (!send_buffer(tx_hid_buf_)) {
    return false;
  }

  return true;
}

bool Protocol::get_curve_(CurveCommandParameters curve_command) {
  Curve *curve = nullptr;
  if (curve_command.curve_type == CurveType::FAN_CURVE) {
    if (curve_command.channel >= Constants::NUM_FANS) {
      return false;
    }
    auto &fan_control = Config::get_fan_control(curve_command.channel);
    auto &temp_curve = fan_control.get_curve();
    curve = &temp_curve;
  } else if (curve_command.curve_type == CurveType::LED_CURVE) {
    if (curve_command.channel >= Constants::NUM_LEDS) {
      return false;
    }
    auto &curves = Config::get_led_control(curve_command.channel).get_curves();
    switch (curve_command.rgb_channel) {
    case CurveChannel::R_CHAN:
      curve = &curves[0];
      break;
    case CurveChannel::G_CHAN:
      curve = &curves[1];
      break;
    case CurveChannel::B_CHAN:
      curve = &curves[2];
      break;
    default:
      return false;
    }
  } else {
    return false;
  }

  const auto num_packets =
      ceil(((float)sizeof(CurveCommandParameters) +
            sizeof(float) * 2 * curve->get_valid_points()) /
           (float)RAWHID_TX_SIZE);
  if (!send_start_packet_(Command::GET_CURVE, num_packets)) {
    return false;
  }

  clear_buffer(tx_hid_buf_);
  curve_command.curve_length = curve->get_valid_points();
  const auto &points = curve->get_points_const();
  memcpy(tx_hid_buf_.data(), &curve_command, sizeof(CurveCommandParameters));
  uint16_t packet_offset = sizeof(CurveCommandParameters);
  for (uint i = 0; i < curve_command.curve_length; i++) {
    auto x = (float)std::get<X>(points[i]);
    auto y = (float)std::get<Y>(points[i]);
    memcpy(tx_hid_buf_.data() + packet_offset, &x, sizeof(float));
    memcpy(tx_hid_buf_.data() + packet_offset + sizeof(float), &y,
           sizeof(float));

    packet_offset += sizeof(float) * 2;
    if (packet_offset + sizeof(float) * 2 >= RAWHID_TX_SIZE) {
      if (!send_buffer(tx_hid_buf_)) {
        return false;
      }
      packet_offset = 0;
      clear_buffer(tx_hid_buf_);
    }
  }

  if (packet_offset > 0) {
    if (!send_buffer(tx_hid_buf_)) {
      return false;
    }
  }

  return true;
}

bool Protocol::set_curve_(CurveCommandParameters curve_command) {
  Curve *curve = nullptr;
  if (curve_command.curve_type == CurveType::FAN_CURVE) {
    if (curve_command.channel >= Constants::NUM_FANS) {
      return false;
    }
    auto &fan_control = Config::get_fan_control(curve_command.channel);
    auto &temp_curve = fan_control.get_curve();
    curve = &temp_curve;
  } else if (curve_command.curve_type == CurveType::LED_CURVE) {
    if (curve_command.channel >= Constants::NUM_LEDS) {
      return false;
    }
    auto &curves = Config::get_led_control(curve_command.channel).get_curves();
    switch (curve_command.rgb_channel) {
    case CurveChannel::R_CHAN:
      curve = &curves[0];
      break;
    case CurveChannel::G_CHAN:
      curve = &curves[1];
      break;
    case CurveChannel::B_CHAN:
      curve = &curves[2];
      break;
    default:
      return false;
    }
  } else {
    return false;
  }

  clear_buffer(rx_hid_buf_);
  if (RawHID.recv(rx_hid_buf_.data(), HID_TIMEOUT) <= 0) {
    return false;
  }
  auto &points = curve->get_points();
  uint16_t packet_offset = 0;
  for (uint i = 0; i < curve_command.curve_length; i++) {
    float x, y;
    memcpy(&x, rx_hid_buf_.data() + packet_offset, sizeof(float));
    memcpy(&y, rx_hid_buf_.data() + packet_offset + sizeof(float),
           sizeof(float));
    points[i] = std::move(std::make_tuple(x, y));

    packet_offset += sizeof(float) * 2;
    if (packet_offset + sizeof(float) * 2 >= RAWHID_TX_SIZE) {
      clear_buffer(rx_hid_buf_);
      if (RawHID.recv(rx_hid_buf_.data(), HID_TIMEOUT) <= 0) {
        return false;
      }
      packet_offset = 0;
    }
  }

  curve->set_valid_points(curve_command.curve_length);

  clear_buffer(tx_hid_buf_);
  memcpy(tx_hid_buf_.data(), &curve_command, sizeof(CurveCommandParameters));
  if (!send_buffer(tx_hid_buf_)) {
    return false;
  }

  return true;
}

bool Protocol::send_all_rpm_() {
  const auto num_packets =
      ceil((float)sizeof(float) * (float)Constants::NUM_FANS /
           (float)RAWHID_TX_SIZE);
  if (!send_start_packet_(Command::GET_ALL_RPM, num_packets)) {
    return false;
  }

  clear_buffer(tx_hid_buf_);
  uint16_t packet_offset = 0;
  for (uint i = 0; i < Constants::NUM_FANS; i++) {
    auto rpm = LB::Config::get_fan(i).read_rpm();
    memcpy(tx_hid_buf_.data() + packet_offset, &rpm, sizeof(float));
    packet_offset += sizeof(float);
    if (packet_offset + sizeof(float) >= RAWHID_TX_SIZE) {
      if (!send_buffer(tx_hid_buf_)) {
        return false;
      }
      packet_offset = 0;
      clear_buffer(tx_hid_buf_);
    }
  }
  if (packet_offset > 0) {
    if (!send_buffer(tx_hid_buf_)) {
      return false;
    }
  }

  return true;
}

bool Protocol::send_all_temperatures_() {
  const auto num_packets =
      ceil((float)sizeof(float) * (float)Constants::NUM_TEMPERATURE_SENSORS /
           (float)RAWHID_TX_SIZE);
  if (!send_start_packet_(Command::GET_ALL_TEMPERATURE, num_packets)) {
    return false;
  }

  clear_buffer(tx_hid_buf_);
  uint16_t packet_offset = 0;
  for (uint i = 0; i < Constants::NUM_TEMPERATURE_SENSORS; i++) {
    auto temp = LB::Config::get_temperature_sensor(i).read_temperature();
    memcpy(tx_hid_buf_.data() + packet_offset, &temp, sizeof(float));
    packet_offset += sizeof(float);
    if (packet_offset + sizeof(float) >= RAWHID_TX_SIZE) {
      if (!send_buffer(tx_hid_buf_)) {
        return false;
      }
      packet_offset = 0;
      clear_buffer(tx_hid_buf_);
    }
  }
  if (packet_offset > 0) {
    if (!send_buffer(tx_hid_buf_)) {
      return false;
    }
  }

  return true;
}

bool Protocol::get_all_fan_parameters_() {
  const auto num_packets =
      ceil((float)sizeof(FanParameters) * (float)Constants::NUM_FANS /
           (float)RAWHID_TX_SIZE);
  if (!send_start_packet_(Command::GET_ALL_FAN_PARAMETERS, num_packets)) {
    return false;
  }

  clear_buffer(tx_hid_buf_);
  uint16_t packet_offset = 0;
  for (uint i = 0; i < Constants::NUM_FANS; i++) {
    auto &fan = LB::Config::get_fan(i);
    const FanParameters fan_params = {.channel = i,
                                      .pwm_controlled = fan.is_pwm_controlled(),
                                      .pwm = fan.get_pwm(),
                                      .voltage = fan.get_voltage()};
    memcpy(tx_hid_buf_.data() + packet_offset, &fan_params,
           sizeof(FanParameters));
    packet_offset += sizeof(FanParameters);
    if (packet_offset + sizeof(FanParameters) >= RAWHID_TX_SIZE) {
      if (!send_buffer(tx_hid_buf_)) {
        return false;
      }
      packet_offset = 0;
      clear_buffer(tx_hid_buf_);
    }
  }

  if (packet_offset > 0) {
    if (!send_buffer(tx_hid_buf_)) {
      return false;
    }
  }

  return true;
}

bool Protocol::set_fan_parameters_(FanParameters fan_command) {
  Curve *curve = nullptr;
  if (fan_command.channel >= Constants::NUM_FANS) {
    return false;
  }

  auto &fan_control = Config::get_fan_control(fan_command.channel);
  auto &fan = Config::get_fan(fan_command.channel);

  // if (fan_command.pwm_controlled) {
  //   fan.set_pwm(fan_command.pwm);
  // } else {
  //   fan.set_voltage(fan_command.voltage);
  // }

  fan.set_pwm_controlled(fan_command.pwm_controlled);

  clear_buffer(tx_hid_buf_);
  memcpy(tx_hid_buf_.data(), &fan_command, sizeof(FanParameters));
  if (!send_buffer(tx_hid_buf_)) {
    return false;
  }

  return true;
}

bool Protocol::get_all_led_parameters_() {
  const auto num_packets =
      ceil((float)sizeof(LEDParameters) * (float)Constants::NUM_LEDS /
           (float)RAWHID_TX_SIZE);
  if (!send_start_packet_(Command::GET_ALL_LED_PARAMETERS, num_packets)) {
    return false;
  }

  clear_buffer(tx_hid_buf_);
  uint16_t packet_offset = 0;
  for (uint i = 0; i < Constants::NUM_LEDS; i++) {
    auto &led_control = LB::Config::get_led_control(i);
    auto &led = LB::Config::get_led(i);
    const LEDParameters led_params = {
        .channel = i,
        .time_controlled = led_control.is_time_controlled(),
        .speed_multiplier = (float)led_control.get_speed(),
        .r_brightness = led.get_r_brightness(),
        .g_brightness = led.get_g_brightness(),
        .b_brightness = led.get_b_brightness()};
    memcpy(tx_hid_buf_.data() + packet_offset, &led_params,
           sizeof(LEDParameters));
    packet_offset += sizeof(LEDParameters);
    if (packet_offset + sizeof(LEDParameters) >= RAWHID_TX_SIZE) {
      if (!send_buffer(tx_hid_buf_)) {
        return false;
      }
      packet_offset = 0;
      clear_buffer(tx_hid_buf_);
    }
  }

  if (packet_offset > 0) {
    if (!send_buffer(tx_hid_buf_)) {
      return false;
    }
  }

  return true;
}

bool Protocol::set_led_parameters_(LEDParameters led_command) {
  Curve *curve = nullptr;
  if (led_command.channel >= Constants::NUM_LEDS) {
    return false;
  }

  auto &led_control = Config::get_led_control(led_command.channel);
  auto &led = Config::get_led(led_command.channel);

  led_control.set_time_controlled(led_command.time_controlled);
  led_control.set_speed(led_command.speed_multiplier);

  clear_buffer(tx_hid_buf_);
  memcpy(tx_hid_buf_.data(), &led_command, sizeof(LEDParameters));
  if (!send_buffer(tx_hid_buf_)) {
    return false;
  }

  return true;
}

bool Protocol::send_start_packet_(Command command, uint32_t num_packets) {
  clear_buffer(tx_hid_buf_);
  tx_hid_buf_[0] = (uint8_t)command;
  memcpy(tx_hid_buf_.data() + 1, &num_packets, sizeof(uint32_t));
  if (RawHID.send(tx_hid_buf_.data(), HID_TIMEOUT) != RAWHID_TX_SIZE) {
#ifndef DISABLE_SERIAL
    Serial.println(F("Failed to send start packet"));
    return false;
#endif
  } else {
#ifndef DISABLE_SERIAL
    Serial.println(F("Sent package"));
#endif
  }
  return true;
}

void Protocol::loop() {
  if (!check_delay_.justFinished()) {
    return;
  }
  check_delay_.repeat();

  if (!RawHID.available()) {
    return;
  }

  clear_buffer(rx_hid_buf_);

  if (RawHID.recv(rx_hid_buf_.data(), HID_TIMEOUT) <= 0) {
    return;
  }

  const auto command = (Command)rx_hid_buf_[0];
  print_command_(command);

  process_command_(command, rx_hid_buf_, 1);
}
} // namespace LB
