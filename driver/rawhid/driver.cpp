#include "driver.h"

#include <cstdbool>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <tuple>
#include <vector>

#include "hid.h"

#include "nlohmann/json.hpp"
// #include "hidapi/hidapi.h"

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

bool Driver::disconnect() {
  if (driver_handle_ == DEFAULT_DRIVER_HANDLE) {
    return true;
  }

  rawhid_close(driver_handle_);
  driver_handle_ = DEFAULT_DRIVER_HANDLE;
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
  if (command != Command::GET_CURVE) {
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
    float x, y;
    memcpy(&x, trx_buf_.data() + packet_offset, sizeof(float));
    memcpy(&y, trx_buf_.data() + packet_offset + sizeof(float), sizeof(float));
    points.push_back(std::make_tuple(x, y));

    packet_offset += sizeof(float) * 2;
    if (packet_offset + sizeof(float) * 2 >= HID_BUF_SIZE) {
      RECEIVE_BUF(trx_buf_, {});

      packet_offset = 0;
    }
  }

  return points;
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

std::string Driver::get_all_fan_rpms() {
  memset(trx_buf_.data(), 0, sizeof(uint8_t) * trx_buf_.size());
  trx_buf_[0] = (uint8_t)Command::GET_ALL_RPM;
  rawhid_send(driver_handle_, trx_buf_.data(), trx_buf_.size(), HID_TIMEOUT);

  RECEIVE_BUF(trx_buf_, {});

  uint8_t command;
  memcpy(&command, trx_buf_.data(), sizeof(uint8_t));
  if (command != Command::GET_ALL_RPM) {
    return {};
  }

  std::array<float, Constants::NUM_FANS> fan_rpms = {};
  uint16_t packet_offset = 0;
  RECEIVE_BUF(trx_buf_, {});
  for (uint i = 0; i < Constants::NUM_FANS; i++) {
    float rpm;
    memcpy(&rpm, trx_buf_.data() + packet_offset, sizeof(float));
    fan_rpms[i] = rpm;

    packet_offset += sizeof(float);
    if (packet_offset + sizeof(float) >= HID_BUF_SIZE) {
      RECEIVE_BUF(trx_buf_, {});
      packet_offset = 0;
    }
  }

  nlohmann::json j;
  j["rpms"] = fan_rpms;
  return j.dump();
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

std::string Driver::get_all_temperatures() {
  memset(trx_buf_.data(), 0, sizeof(uint8_t) * trx_buf_.size());
  trx_buf_[0] = (uint8_t)Command::GET_ALL_TEMPERATURE;
  rawhid_send(driver_handle_, trx_buf_.data(), trx_buf_.size(), HID_TIMEOUT);

  RECEIVE_BUF(trx_buf_, {});

  uint8_t command;
  memcpy(&command, trx_buf_.data(), sizeof(uint8_t));
  if (command != Command::GET_ALL_TEMPERATURE) {
    return {};
  }

  std::array<float, Constants::NUM_TEMPERATURE_SENSORS> temperatures = {};
  uint16_t packet_offset = 0;
  RECEIVE_BUF(trx_buf_, {});
  for (uint i = 0; i < Constants::NUM_TEMPERATURE_SENSORS; i++) {
    float temperature;
    memcpy(&temperature, trx_buf_.data() + packet_offset, sizeof(float));
    temperatures[i] = temperature;

    packet_offset += sizeof(float);
    if (packet_offset + sizeof(float) >= HID_BUF_SIZE) {
      RECEIVE_BUF(trx_buf_, {});
      packet_offset = 0;
    }
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
  if (command != Command::GET_ALL_FAN_PARAMETERS) {
    return {};
  }

  std::array<FanParameters, Constants::NUM_FANS> all_fans = {};
  uint16_t packet_offset = 0;
  RECEIVE_BUF(trx_buf_, {});
  for (uint i = 0; i < Constants::NUM_FANS; i++) {
    FanParameters fan_params;
    memcpy(&fan_params, trx_buf_.data() + packet_offset, sizeof(FanParameters));
    all_fans[i] = fan_params;

    packet_offset += sizeof(FanParameters);
    if (packet_offset + sizeof(FanParameters) >= HID_BUF_SIZE) {
      RECEIVE_BUF(trx_buf_, {});
      packet_offset = 0;
    }
  }

  nlohmann::json j;
  for (auto &fan_params : all_fans) {
    j["fans"].push_back(
        nlohmann::json::object({{"channel", fan_params.channel},
                                {"pwm_controlled", fan_params.pwm_controlled},
                                {"pwm", fan_params.pwm},
                                {"voltage", fan_params.voltage}}));
  }
  return j.dump();
}

std::string Driver::get_all_led_parameters() {
  memset(trx_buf_.data(), 0, sizeof(uint8_t) * trx_buf_.size());
  trx_buf_[0] = (uint8_t)Command::GET_ALL_LED_PARAMETERS;
  rawhid_send(driver_handle_, trx_buf_.data(), trx_buf_.size(), HID_TIMEOUT);

  RECEIVE_BUF(trx_buf_, {});

  uint8_t command;
  memcpy(&command, trx_buf_.data(), sizeof(uint8_t));
  if (command != Command::GET_ALL_LED_PARAMETERS) {
    return {};
  }

  std::array<LEDParameters, Constants::NUM_LEDS> all_leds = {};
  uint16_t packet_offset = 0;
  RECEIVE_BUF(trx_buf_, {});
  for (uint i = 0; i < Constants::NUM_LEDS; i++) {
    LEDParameters led_params;
    memcpy(&led_params, trx_buf_.data() + packet_offset, sizeof(LEDParameters));
    all_leds[i] = led_params;

    packet_offset += sizeof(LEDParameters);
    if (packet_offset + sizeof(LEDParameters) >= HID_BUF_SIZE) {
      RECEIVE_BUF(trx_buf_, {});
      packet_offset = 0;
    }
  }

  nlohmann::json j;
  for (auto &led_params : all_leds) {
    j["leds"].push_back(nlohmann::json::object(
        {{"channel", led_params.channel},
         {"time_controlled", led_params.time_controlled},
         {"speed_multiplier", led_params.speed_multiplier},
         {"r_brightness", led_params.r_brightness},
         {"g_brightness", led_params.g_brightness},
         {"b_brightness", led_params.b_brightness}}));
  }
  return j.dump();
}
} // namespace LB
