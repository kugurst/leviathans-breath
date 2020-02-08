#include "driver.hpp"

#include <cstdbool>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <tuple>
#include <vector>

#ifdef TEST
#include "hid.h"
#else
extern "C" {
#include "hid.h"
}
#endif

#include "nlohmann/json.hpp"

#define RECEIVE_BUF(buf, ret)                                                  \
  {                                                                            \
    memset(buf.data(), 0, sizeof(uint8_t) * buf.size());                       \
    auto bytes_received =                                                      \
        rawhid_recv(driver_handle_, buf.data(), buf.size(), HID_TIMEOUT);      \
    print_bytes_received(bytes_received, buf);                                 \
    if (bytes_received <= 0) {                                                 \
      return ret;                                                              \
    }                                                                          \
  }

#define SEND_BUF(buf, ret)                                                     \
  {                                                                            \
    if (rawhid_send(driver_handle_, buf.data(), buf.size(), HID_TIMEOUT) <=    \
        0) {                                                                   \
      return ret;                                                              \
    }                                                                          \
    memset(buf.data(), 0, sizeof(uint8_t) * buf.size());                       \
  }

namespace LB {
static void print_bytes_received(int bytes_received,
                                 std::array<uint8_t, HID_BUF_SIZE> &buf) {
#ifdef DEBUG
  printf("\nrecv %d bytes:\n", bytes_received);
  for (auto i = 0; i < bytes_received; i++) {
    printf("%02X ", buf[i] & 255);
    if (i % 16 == 15 && i < bytes_received - 1)
      printf("\n");
  }
  printf("\n");
#endif
}

bool Driver::connect() {
  if (driver_handle_ != DEFAULT_DRIVER_HANDLE) {
    return true;
  }

  // C-based example is 16C0:0480:FFAB:0200
  auto num_opened = rawhid_open(1, 0x16C0, 0x0480, 0xFFAB, 0x0200);
  if (num_opened <= 0) {
    // Arduino-based example is 16C0:0486:FFAB:0200
    num_opened = rawhid_open(1, 0x16C0, 0x0486, 0xFFAB, 0x0200);
    if (num_opened <= 0) {
#ifdef DEBUG
      std::cout << "no rawhid device found" << std::endl;
#endif
      return false;
    }
  }

  driver_handle_ = 0;

  srand((unsigned int)time(NULL));

#ifdef DEBUG
  std::cout << "found rawhid device" << std::endl;
#endif
  return true;
}

bool Driver::sync() {
  std::array<uint8_t, HID_BUF_SIZE - 1> rand_dat;
  for (auto i = 0; i < rand_dat.size(); i++) {
    rand_dat[i] = rand();
  }

  memset(trx_buf_.data(), 0, sizeof(uint8_t) * trx_buf_.size());
  trx_buf_[0] = (uint8_t)Command::ECHO;
  memcpy(trx_buf_.data() + 1, rand_dat.data(), rand_dat.size());

  // print_bytes_received(trx_buf_.size(), trx_buf_);

  rawhid_send(driver_handle_, trx_buf_.data(), trx_buf_.size(), HID_TIMEOUT);

  auto retry_count = 0;
  while (true) {
    RECEIVE_BUF(trx_buf_, false);

    if (memcmp(trx_buf_.data() + 1, rand_dat.data(), rand_dat.size()) == 0) {
      break;
    }

    retry_count++;
    if (retry_count >= SYNC_RETRY_COUNT) {
      return false;
    }
  }

  return true;
}

std::vector<std::tuple<float, float>>
Driver::get_curve_(CurveCommandParameters curve_command) {
  memset(trx_buf_.data(), 0, sizeof(uint8_t) * trx_buf_.size());
  trx_buf_[0] = (uint8_t)Command::GET_CURVE;
  memcpy(trx_buf_.data() + 1, &curve_command, sizeof(CurveCommandParameters));
  rawhid_send(driver_handle_, trx_buf_.data(), trx_buf_.size(), HID_TIMEOUT);

  RECEIVE_BUF(trx_buf_, {});

  uint8_t command;
  memcpy(&command, trx_buf_.data(), sizeof(uint8_t));
  if (command != static_cast<uint8_t>(Command::GET_CURVE)) {
    return {};
  }

  RECEIVE_BUF(trx_buf_, {});

  memcpy(&curve_command, trx_buf_.data(), sizeof(CurveCommandParameters));
#ifdef DEBUG
  printf("channel %u, curve length: %u\n", curve_command.channel,
         curve_command.curve_length);
#endif

  uint16_t packet_offset = sizeof(CurveCommandParameters);
  std::vector<std::tuple<float, float>> points;
  points.reserve(curve_command.curve_length);
  for (uint i = 0; i < curve_command.curve_length; i++) {
    if (packet_offset + sizeof(float) * 2 >= HID_BUF_SIZE) {
      RECEIVE_BUF(trx_buf_, {});
      packet_offset = 0;
    }

    float x, y;
    memcpy(&x, trx_buf_.data() + packet_offset, sizeof(float));
    memcpy(&y, trx_buf_.data() + packet_offset + sizeof(float), sizeof(float));
    points.push_back(std::make_tuple(x, y));

    packet_offset += sizeof(float) * 2;
  }

  return points;
}

bool Driver::send_curve_(CurveCommandParameters curve_command,
                         const std::vector<std::tuple<float, float>> &curve) {
  memset(trx_buf_.data(), 0, sizeof(uint8_t) * trx_buf_.size());
  trx_buf_[0] = (uint8_t)Command::SET_CURVE;
  memcpy(trx_buf_.data() + 1, &curve_command, sizeof(CurveCommandParameters));
  SEND_BUF(trx_buf_, false);

  uint16_t packet_offset = 0;
  for (uint i = 0; i < curve_command.curve_length; i++) {
    const auto &point = curve[i];
    const auto x = std::get<0>(point);
    const auto y = std::get<1>(point);
    memcpy(trx_buf_.data() + packet_offset, &x, sizeof(float));
    memcpy(trx_buf_.data() + packet_offset + sizeof(float), &y, sizeof(float));

    packet_offset += sizeof(float) * 2;
    if (packet_offset + sizeof(float) * 2 >= HID_BUF_SIZE) {
      SEND_BUF(trx_buf_, false);

      packet_offset = 0;
    }
  }
  if (packet_offset > 0) {
    SEND_BUF(trx_buf_, false);
  }

  CurveCommandParameters ack;
  RECEIVE_BUF(trx_buf_, false);
  memcpy(&ack, trx_buf_.data(), sizeof(CurveCommandParameters));

  return ack.curve_length == curve_command.curve_length;
}

std::string Driver::get_fan_curve(int channel) {
  if (channel >= Constants::NUM_FANS) {
    return {};
  }

  CurveCommandParameters curve_command = {.channel =
                                              static_cast<uint8_t>(channel),
                                          .curve_type = CurveType::FAN_CURVE};

  auto points = get_curve_(curve_command);
  if (points.size() == 0) {
    return {};
  }

  nlohmann::json j;
  j["points"] = points;
  return j.dump();
}

bool Driver::send_fan_curve(int channel, const std::string &curve) {
  if (channel >= Constants::NUM_FANS) {
    return false;
  }

  nlohmann::json j = nlohmann::json::parse(curve);
  auto points = j["points"].get<std::vector<std::tuple<float, float>>>();

  if (points.size() > Constants::POINTS_PER_CURVE) {
    return false;
  }

  CurveCommandParameters curve_command = {
      .channel = static_cast<uint8_t>(channel),
      .curve_type = CurveType::FAN_CURVE,
      .curve_length = static_cast<uint16_t>(points.size())};

  return send_curve_(curve_command, points);
}

std::string Driver::get_led_curve(int channel) {
  if (channel >= Constants::NUM_LEDS) {
    return {};
  }

  std::array<std::vector<std::tuple<float, float>>, Constants::NUM_LED_CHANNELS>
      curves = {};

  for (auto i = 0; i < Constants::NUM_LED_CHANNELS; i++) {
    CurveCommandParameters curve_command = {.channel =
                                                static_cast<uint8_t>(channel),
                                            .curve_type = CurveType::LED_CURVE,
                                            .rgb_channel = (CurveChannel)i};

    curves[i] = get_curve_(curve_command);
  }

  nlohmann::json j;
  j["points"]["r"] = curves[0];
  j["points"]["g"] = curves[1];
  j["points"]["b"] = curves[2];
  return j.dump();
}

bool Driver::send_led_curve(int channel, const std::string &curve) {
  if (channel >= Constants::NUM_LEDS) {
    return false;
  }

  nlohmann::json j = nlohmann::json::parse(curve);
  std::array<std::vector<std::tuple<float, float>>, Constants::NUM_LED_CHANNELS>
      curves;
  curves[0] = j["points"]["r"].get<std::vector<std::tuple<float, float>>>();
  curves[1] = j["points"]["g"].get<std::vector<std::tuple<float, float>>>();
  curves[2] = j["points"]["b"].get<std::vector<std::tuple<float, float>>>();

  for (const auto &temp_curve : curves) {
    if (temp_curve.size() > Constants::POINTS_PER_CURVE) {
      return false;
    }
  }

  auto all_successful = true;
  for (auto i = 0; i < Constants::NUM_LED_CHANNELS; i++) {
    const auto &temp_curve = curves[i];
    CurveCommandParameters curve_command = {
        .channel = static_cast<uint8_t>(channel),
        .curve_type = CurveType::LED_CURVE,
        .rgb_channel = (CurveChannel)i,
        .curve_length = static_cast<uint16_t>(temp_curve.size())};

    if (!send_curve_(curve_command, temp_curve)) {
      all_successful = false;
    }
  }

  return all_successful;
}

std::string Driver::get_all_fan_rpms() {
  memset(trx_buf_.data(), 0, sizeof(uint8_t) * trx_buf_.size());
  trx_buf_[0] = (uint8_t)Command::GET_ALL_RPM;
  rawhid_send(driver_handle_, trx_buf_.data(), trx_buf_.size(), HID_TIMEOUT);

  RECEIVE_BUF(trx_buf_, {});

  uint8_t command;
  memcpy(&command, trx_buf_.data(), sizeof(uint8_t));
  if (command != static_cast<uint8_t>(Command::GET_ALL_RPM)) {
    return {};
  }

  std::array<float, Constants::NUM_FANS> fan_rpms = {};
  uint16_t packet_offset = 0;
  RECEIVE_BUF(trx_buf_, {});
  for (uint i = 0; i < Constants::NUM_FANS; i++) {
    if (packet_offset + sizeof(float) >= HID_BUF_SIZE) {
      RECEIVE_BUF(trx_buf_, {});
      packet_offset = 0;
    }

    float rpm;
    memcpy(&rpm, trx_buf_.data() + packet_offset, sizeof(float));
    fan_rpms[i] = rpm;

    packet_offset += sizeof(float);
  }

  nlohmann::json j;
  j["rpms"] = fan_rpms;
  return j.dump();
}

std::string Driver::get_all_temperatures() {
  memset(trx_buf_.data(), 0, sizeof(uint8_t) * trx_buf_.size());
  trx_buf_[0] = (uint8_t)Command::GET_ALL_TEMPERATURE;
  rawhid_send(driver_handle_, trx_buf_.data(), trx_buf_.size(), HID_TIMEOUT);

  RECEIVE_BUF(trx_buf_, {});

  uint8_t command;
  memcpy(&command, trx_buf_.data(), sizeof(uint8_t));
  if (command != static_cast<uint8_t>(Command::GET_ALL_TEMPERATURE)) {
    return {};
  }

  std::array<float, Constants::NUM_TEMPERATURE_SENSORS> temperatures = {};
  uint16_t packet_offset = 0;
  RECEIVE_BUF(trx_buf_, {});
  for (uint i = 0; i < Constants::NUM_TEMPERATURE_SENSORS; i++) {
    if (packet_offset + sizeof(float) >= HID_BUF_SIZE) {
      RECEIVE_BUF(trx_buf_, {});
      packet_offset = 0;
    }

    float temperature;
    memcpy(&temperature, trx_buf_.data() + packet_offset, sizeof(float));
    temperatures[i] = temperature;

    packet_offset += sizeof(float);
  }

  nlohmann::json j;
  j["temperatures"] = temperatures;
  return j.dump();
}

std::string Driver::get_all_fan_parameters() {
  memset(trx_buf_.data(), 0, sizeof(uint8_t) * trx_buf_.size());
  trx_buf_[0] = (uint8_t)Command::GET_ALL_FAN_PARAMETERS;
  rawhid_send(driver_handle_, trx_buf_.data(), trx_buf_.size(), HID_TIMEOUT);

  RECEIVE_BUF(trx_buf_, {});

  uint8_t command;
  memcpy(&command, trx_buf_.data(), sizeof(uint8_t));
  if (command != static_cast<uint8_t>(Command::GET_ALL_FAN_PARAMETERS)) {
    return {};
  }

  std::array<FanParameters, Constants::NUM_FANS> all_fans = {};
  uint16_t packet_offset = 0;
  RECEIVE_BUF(trx_buf_, {});
  for (uint i = 0; i < Constants::NUM_FANS; i++) {
    if (packet_offset + sizeof(FanParameters) >= HID_BUF_SIZE) {
      RECEIVE_BUF(trx_buf_, {});
      packet_offset = 0;
    }

    FanParameters fan_params;
    memcpy(&fan_params, trx_buf_.data() + packet_offset, sizeof(FanParameters));
    all_fans[i] = fan_params;

    packet_offset += sizeof(FanParameters);
  }

  nlohmann::json j;
  for (auto &fan_params : all_fans) {
    j["fans"].push_back(nlohmann::json::object(
        {{"channel", fan_params.channel},
         {"pwm_controlled", fan_params.pwm_controlled},
         {"temperature_channel", fan_params.temperature_channel},
         {"pwm", fan_params.pwm},
         {"voltage", fan_params.voltage}}));
  }
  return j.dump();
}

bool Driver::set_fan_parameters(int channel, bool pwm_controlled,
                                int temperature_sensor_idx) {
  if (channel >= Constants::NUM_FANS) {
    return false;
  }
  if (temperature_sensor_idx >= Constants::NUM_TEMPERATURE_SENSORS) {
    return false;
  }

  FanParameters fan_params = {.channel = static_cast<uint8_t>(channel),
                              .pwm_controlled = pwm_controlled,
                              .temperature_channel =
                                  static_cast<uint8_t>(temperature_sensor_idx)};

  memset(trx_buf_.data(), 0, sizeof(uint8_t) * trx_buf_.size());
  trx_buf_[0] = (uint8_t)Command::SET_FAN_PARAMETERS;
  memcpy(trx_buf_.data() + 1, &fan_params, sizeof(FanParameters));

  SEND_BUF(trx_buf_, false);

  FanParameters ack;
  RECEIVE_BUF(trx_buf_, false);
  memcpy(&ack, trx_buf_.data(), sizeof(FanParameters));

  return ack.channel == fan_params.channel &&
         ack.pwm_controlled == fan_params.pwm_controlled &&
         ack.temperature_channel == fan_params.temperature_channel;
}

std::string Driver::get_all_led_parameters() {
  memset(trx_buf_.data(), 0, sizeof(uint8_t) * trx_buf_.size());
  trx_buf_[0] = (uint8_t)Command::GET_ALL_LED_PARAMETERS;
  rawhid_send(driver_handle_, trx_buf_.data(), trx_buf_.size(), HID_TIMEOUT);

  RECEIVE_BUF(trx_buf_, {});

  uint8_t command;
  memcpy(&command, trx_buf_.data(), sizeof(uint8_t));
  if (command != static_cast<uint8_t>(Command::GET_ALL_LED_PARAMETERS)) {
    return {};
  }

  std::array<LEDParameters, Constants::NUM_LEDS> all_leds = {};
  uint16_t packet_offset = 0;
  RECEIVE_BUF(trx_buf_, {});
  for (uint i = 0; i < Constants::NUM_LEDS; i++) {
    if (packet_offset + sizeof(LEDParameters) >= HID_BUF_SIZE) {
      RECEIVE_BUF(trx_buf_, {});
      packet_offset = 0;
    }

    LEDParameters led_params;
    memcpy(&led_params, trx_buf_.data() + packet_offset, sizeof(LEDParameters));
    all_leds[i] = led_params;

    packet_offset += sizeof(LEDParameters);
  }

  nlohmann::json j;
  for (auto &led_params : all_leds) {
    j["leds"].push_back(nlohmann::json::object(
        {{"channel", led_params.channel},
         {"time_controlled", led_params.time_controlled},
         {"speed_multiplier", led_params.speed_multiplier},
         {"temperature_channel", led_params.temperature_channel},
         {"r_brightness", led_params.r_brightness},
         {"g_brightness", led_params.g_brightness},
         {"b_brightness", led_params.b_brightness}}));
  }
  return j.dump();
}

bool Driver::set_led_parameters(int channel, bool time_controlled,
                                float speed_multiplier,
                                int temperature_sensor_idx) {
  if (channel > Constants::NUM_FANS) {
    return false;
  }

  LEDParameters led_params = {.channel = static_cast<uint8_t>(channel),
                              .time_controlled = time_controlled,
                              .speed_multiplier = speed_multiplier,
                              .temperature_channel =
                                  static_cast<int8_t>(temperature_sensor_idx)};

  memset(trx_buf_.data(), 0, sizeof(uint8_t) * trx_buf_.size());
  trx_buf_[0] = (uint8_t)Command::SET_LED_PARAMETERS;
  memcpy(trx_buf_.data() + 1, &led_params, sizeof(LEDParameters));

  SEND_BUF(trx_buf_, false);

  LEDParameters ack;
  RECEIVE_BUF(trx_buf_, false);
  memcpy(&ack, trx_buf_.data(), sizeof(LEDParameters));

  return ack.channel == led_params.channel &&
         ack.time_controlled == led_params.time_controlled &&
         ack.temperature_channel == led_params.temperature_channel;
}
} // namespace LB
