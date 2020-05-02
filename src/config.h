#pragma once

#include <cstdint>

#include <array>
#include <memory>

#include "constants.hpp"
#include "fan.h"
#include "led.h"
#include "led_contol.h"
#include "temperture.h"
#include "fan_control.h"

namespace LB {
struct FanConfig {
  const uint8_t tach_pin;
  const uint8_t pwm_pin;
  const uint8_t gd_pin;
  const bool pwm_control;
};

struct TemperatureConfig {
  const uint8_t analog_pin;
  const TemperatureProbeType probe_type;
};

struct LedChannelConfig {
  const uint8_t gd_pin;
};

struct LedConfig {
  const LedChannelConfig r_channel;
  const LedChannelConfig g_channel;
  const LedChannelConfig b_channel;
};

class Config {
public:
  static constexpr ::std::array<FanConfig, Constants::NUM_FANS> fan_configs = {
      {{.tach_pin = 31, .pwm_pin = 2, .gd_pin = 8, .pwm_control = true},
       {.tach_pin = 32, .pwm_pin = 3, .gd_pin = 9, .pwm_control = true},
       {.tach_pin = 33, .pwm_pin = 4, .gd_pin = 10, .pwm_control = true},
       {.tach_pin = 34, .pwm_pin = 5, .gd_pin = 29, .pwm_control = true},
       {.tach_pin = 35, .pwm_pin = 6, .gd_pin = 30, .pwm_control = true},
       {.tach_pin = 36, .pwm_pin = 7, .gd_pin = 37, .pwm_control = true}}};

  static constexpr ::std::array<LedConfig, Constants::NUM_LEDS> led_configs = {
      {{.r_channel = {.gd_pin = 20},
        .g_channel = {.gd_pin = 17},
        .b_channel = {.gd_pin = 16}},
       {.r_channel = {.gd_pin = 23},
        .g_channel = {.gd_pin = 22},
        .b_channel = {.gd_pin = 21}}}};

  static constexpr ::std::array<TemperatureConfig,
                              Constants::NUM_TEMPERATURE_SENSORS>
      temp_configs = {
          {{.analog_pin = 39, .probe_type = TemperatureProbeType::BARROW},
           {.analog_pin = 38, .probe_type = TemperatureProbeType::BARROW}}};

  static ::std::array<Fan, Constants::NUM_FANS> fans;
  static ::std::array<LED, Constants::NUM_LEDS> leds;
  static ::std::array<Temperature, Constants::NUM_TEMPERATURE_SENSORS>
      temperature_sensors;

  static ::std::array<FanControl, Constants::NUM_FANS> fan_controls;
  static ::std::array<LEDControl, Constants::NUM_LEDS> led_controls;

  static void init();

  static inline Fan &get_fan(uint8_t channel) { return fans[channel]; };
  static inline LED &get_led(uint8_t channel) { return leds[channel]; };

  static inline FanControl &get_fan_control(uint8_t channel) { return fan_controls[channel]; };
  static inline LEDControl &get_led_control(uint8_t channel) { return led_controls[channel]; };
  static inline Temperature &get_temperature_sensor(uint8_t channel) {
    return temperature_sensors[channel];
  };

  static void loop_fans();
  static void loop_temperature_sensors();

  static void loop_fan_controls();
  static void loop_led_controls();

  static void loop_all();

private:
  static void init_pwm_();
  static void init_fans_();
  static void init_leds_();
  static void init_temperature_sensors_();
  static void init_fan_control_();
  static void init_led_control_();
};
} // namespace LB
