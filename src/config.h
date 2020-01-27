#pragma once

#include <cstdint>

#include <array>
#include <memory>

#include <MCP48xx.h>

#include "constants.h"
#include "fan.h"
#include "led.h"
#include "led_contol.h"
#include "temperture.h"
#include "fan_control.h"

namespace LB {
struct FanConfig {
  const uint8_t tach_pin;
  const uint8_t pwm_pin;
  const uint8_t dac_ss_pin;
  const uint8_t dac_channel;
  const bool pwm_control;
};

struct TemperatureConfig {
  const uint8_t analog_pin;
  const TemperatureProbeType probe_type;
};

struct LedChannelConfig {
  const uint8_t dac_ss_pin;
  const uint8_t dac_channel;
};

struct LedConfig {
  const LedChannelConfig r_channel;
  const LedChannelConfig g_channel;
  const LedChannelConfig b_channel;
};

class Config {
public:
  static const uint8_t MOSI_PIN = 28;
  static const uint8_t SCK_PIN = 27;

  static constexpr std::array<FanConfig, Constants::NUM_FANS> fan_configs = {
      {{.tach_pin = 23, .pwm_pin = 2, .dac_ss_pin = 39, .dac_channel = 0, .pwm_control = true},
       {.tach_pin = 22, .pwm_pin = 3, .dac_ss_pin = 39, .dac_channel = 1, .pwm_control = false},
       {.tach_pin = 21, .pwm_pin = 4, .dac_ss_pin = 38, .dac_channel = 0, .pwm_control = false},
       {.tach_pin = 20, .pwm_pin = 5, .dac_ss_pin = 38, .dac_channel = 1, .pwm_control = false},
       {.tach_pin = 19, .pwm_pin = 6, .dac_ss_pin = 37, .dac_channel = 0, .pwm_control = false},
       {.tach_pin = 18, .pwm_pin = 7, .dac_ss_pin = 37, .dac_channel = 1, .pwm_control = false},
       {.tach_pin = 17, .pwm_pin = 8, .dac_ss_pin = 36, .dac_channel = 0, .pwm_control = false},
       {.tach_pin = 16, .pwm_pin = 9, .dac_ss_pin = 36, .dac_channel = 1, .pwm_control = false}}};

  static constexpr std::array<LedConfig, Constants::NUM_LEDS> led_configs = {
      {{.r_channel = {.dac_ss_pin = 35, .dac_channel = 1},
        .g_channel = {.dac_ss_pin = 35, .dac_channel = 0},
        .b_channel = {.dac_ss_pin = 34, .dac_channel = 0}},
       {.r_channel = {.dac_ss_pin = 33, .dac_channel = 1},
        .g_channel = {.dac_ss_pin = 34, .dac_channel = 1},
        .b_channel = {.dac_ss_pin = 33, .dac_channel = 0}}}};

  static constexpr std::array<TemperatureConfig,
                              Constants::NUM_TEMPERATURE_SENSORS>
      temp_configs = {
          {{.analog_pin = 15, .probe_type = TemperatureProbeType::BARROW},
           {.analog_pin = 14, .probe_type = TemperatureProbeType::BARROW}}};

  static std::array<Fan, Constants::NUM_FANS> fans;
  static std::array<LED, Constants::NUM_LEDS> leds;
  static std::array<Temperature, Constants::NUM_TEMPERATURE_SENSORS>
      temperature_sensors;

  static std::array<FanControl, Constants::NUM_FANS> fan_controls;
  static std::array<LEDControl, Constants::NUM_LEDS> led_controls;

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
  static void pre_init_dac_(MCP4822 &dac);
  static void init_dacs_();
  static void init_fans_();
  static void init_leds_();
  static void init_temperature_sensors_();
  static void init_fan_control_();
  static void init_led_control_();

  static std::array<std::unique_ptr<MCP4822>, Constants::NUM_DACS> dacs_;
  static std::array<uint8_t, Constants::NUM_DACS> dac_ss_pins_;
  static std::array<uint8_t, Constants::NUM_DACS> dac_indices_;
};
} // namespace LB
