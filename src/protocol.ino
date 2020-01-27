#include "protocol.h"

#include <cmath>
#include <cstring>
#include <cstdint>

#include "config.h"

namespace LB {

std::array<uint8_t, HID_BUF_SIZE> Protocol::rx_hid_buf_;
std::array<uint8_t, HID_BUF_SIZE> Protocol::tx_hid_buf_;
microsDelay Protocol::check_delay_;

void Protocol::print_command_(Command command) {
#ifndef DISABLE_SERIAL
  switch (command) {
  case Command::SEND_CURVE:
    break;
  case Command::RECEIVE_CURVE:
    break;
  case Command::SEND_ALL_RPM:
    Serial.println(F("SEND ALL RPM Received"));
    break;
  case Command::SEND_ALL_TEMPERATURE:
    Serial.println(F("SEND ALL TEMPERATURE Received"));
  default:
    break;
  }
#endif
}

void clear_buffer(std::array<uint8_t, HID_BUF_SIZE> &buf) {
  memset(buf.data(), 0, sizeof(uint8_t) * buf.size());
}

bool send_buffer(std::array<uint8_t, HID_BUF_SIZE> &buf)
{
  if (RawHID.send(buf.data(), HID_TIMEOUT) != RAWHID_TX_SIZE) {
#ifndef DISABLE_SERIAL
    Serial.println(F("Failed to send buffer"));
    return false;
#endif
  }
  return true;
}

void Protocol::init() { check_delay_.start(CHECK_DELAY_US); }

void Protocol::process_command_(Command command) {
  switch (command) {
  case Command::SEND_CURVE:
    break;
  case Command::RECEIVE_CURVE:
    break;
  case Command::SEND_ALL_RPM:
    send_all_rpm_();
    break;
  case Command::SEND_ALL_TEMPERATURE:
    send_all_temperatures_();
    break;
  default:
#ifndef DISABLE_SERIAL
    Serial.print(F("Unknown command: "));
    Serial.println((uint32_t)command);
#endif
    break;
  }
}

bool Protocol::send_all_rpm_() {
  const auto num_packets =
      ceil((float)sizeof(float) * (float)Constants::NUM_FANS /
           (float)RAWHID_TX_SIZE);
  if (!send_start_packet_(Command::SEND_ALL_RPM, num_packets)) {
    return false;
  }

  clear_buffer(tx_hid_buf_);
  uint16_t packet_offset = 0;
  for (uint i = 0; i < Constants::NUM_FANS; i++) {
    auto rpm = LB::Config::get_fan(i).read_rpm();
    memcpy(tx_hid_buf_.data() + packet_offset, &rpm, sizeof(float));
    packet_offset += sizeof(float);
    if (packet_offset >= RAWHID_TX_SIZE) {
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
  if (!send_start_packet_(Command::SEND_ALL_TEMPERATURE, num_packets)) {
    return false;
  }

  clear_buffer(tx_hid_buf_);
  uint16_t packet_offset = 0;
  for (uint i = 0; i < Constants::NUM_TEMPERATURE_SENSORS; i++) {
    auto temp = LB::Config::get_temperature_sensor(i).read_temperature();
    memcpy(tx_hid_buf_.data() + packet_offset, &temp, sizeof(float));
    packet_offset += sizeof(float);
    if (packet_offset >= RAWHID_TX_SIZE) {
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
    auto& fan = LB::Config::get_fan(i);
    const FanParameters fan_params = {.pwm_controlled = fan.is_pwm_controlled(), .pwm = fan.get_pwm(), .voltage = fan.get_voltage()};
    memcpy(tx_hid_buf_.data() + packet_offset, &fan_params, sizeof(FanParameters));
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

  process_command_(command);
}
} // namespace LB
