#pragma once

#include <array>
#include <string>
#include <tuple>
#include <vector>

#include "constants.hpp"

namespace LB {
const uint8_t SYNC_RETRY_COUNT = 10;
const int DEFAULT_DRIVER_HANDLE = -111;
const uint8_t HID_BUF_SIZE = 64;
const uint16_t HID_TIMEOUT = 1000;

// All commands are issued by the master
enum Command : uint8_t {
  ECHO,
  GET_CURVE,
  SET_CURVE,
  GET_ALL_RPM,
  GET_ALL_TEMPERATURE,
  GET_ALL_FAN_PARAMETERS,
  SET_FAN_PARAMETERS,
  GET_ALL_LED_PARAMETERS,
  SET_LED_PARAMETERS,
};

enum CurveType : uint8_t { FAN_CURVE, LED_CURVE };

enum CurveChannel : uint8_t { R_CHAN, G_CHAN, B_CHAN };

struct CurveCommandParameters {
  uint8_t channel;
  CurveType curve_type;
  CurveChannel rgb_channel;
  uint16_t curve_length;
};

struct FanParameters {
  uint8_t channel;
  bool pwm_controlled;
  uint8_t temperature_channel;
  float pwm;
  float voltage;
};

struct LEDParameters {
  uint8_t channel;
  bool time_controlled;
  float speed_multiplier;
  int8_t temperature_channel;
  float r_brightness;
  float g_brightness;
  float b_brightness;
};

class Driver {
public:
  bool connect();
  inline bool is_connected() { return sync(); };

  bool sync();

  std::string get_fan_curve(int channel);
  bool send_fan_curve(int channel, const std::string &curve);
  std::string get_led_curve(int channel);
  bool send_led_curve(int channel, const std::string &curve);

  std::string get_all_fan_rpms();
  std::string get_all_temperatures();

  std::string get_all_fan_parameters();
  bool set_fan_parameters(int channel, bool pwm_controlled,
                          int temperature_sensor_idx);

  std::string get_all_led_parameters();
  bool set_led_parameters(int channel, bool time_controlled,
                          float speed_multiplier, int temperature_sensor_idx);

private:
  std::vector<std::tuple<float, float>>
  get_curve_(CurveCommandParameters curve_command);
  bool send_curve_(CurveCommandParameters curve_command,
                   const std::vector<std::tuple<float, float>> &curve);

  std::array<uint8_t, HID_BUF_SIZE> trx_buf_;
  int driver_handle_ = DEFAULT_DRIVER_HANDLE;
};
} // namespace LB
